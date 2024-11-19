// Author: Rasoul Nikmehr

#include "csv-client.h"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "common.h"

CsvClient::CsvClient(const char* file_path,
                     std::vector<std::string> search_expressions) {
  this->file_path = file_path;
  this->search_expressions = search_expressions;
}

int CsvClient::execute() {
  mode_t sem_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

  // Number of threads that will be used to search the file lines
  // Each thread will process 1/4 of the lines received from the server via
  // shared memory
  const int NUM_THREADS = 4;

  // A thread semaphore to handle concurrent calls to vector push_back method
  sem_threads = sem_open("/sem__threads", O_CREAT, sem_mode, 1);
  if (sem_threads == SEM_FAILED) {
    print_err("Failed to open the threads semaphore");
    return 1;
  }

  // Unlink the shared memory if already opened
  shm_unlink(&shm_name[0]);

  // Create or open the shared memory for transferring data with the server
  int shm_fd = shm_open(&shm_name[0], O_CREAT | O_RDWR | O_EXCL | S_IRUSR | S_IWUSR, 0666);

  // Size of the structure used to transmit data between client and server
  const size_t shm_size = sizeof(fileDetails);
  if (ftruncate(shm_fd, shm_size) == -1) {
    print_err("Failed to set size of shared memory object");
    shm_unlink(shm_name);
    return 1;
  }

  if (shm_fd == -1) {
    print_err("Failed to open shared memory");
    shm_unlink(shm_name);
    return 1;
  }

  // We pass NULL as the first parameter to allow the OS to select the suitable
  // address for our mapping PROT_READ and PROT_WRITE provide read and write
  // access to the mapped memory segment

  // MAP_SHARED: Specifies that we want the mapped memory be shared between
  // multiple processes (csv-client and csv-server)
  fileDetails* file_details = static_cast<fileDetails*>(
      mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

  if (file_details == MAP_FAILED) {
    print_err("Failed to map the memory");
    return 1;
  }

  // open the barrier semaphore created by the server
  sem_t* sem_barrier = sem_open(SEM_BARRIER_NAME, 0);
  if (sem_barrier == SEM_FAILED) {
    print_err("Failed to open the barrier semaphore");
    return 1;
  }

  sem_t* sem_lines = sem_open(SEM_LINES_NAME, O_CREAT, sem_mode, 0);
  if (sem_lines == SEM_FAILED) {
    print_err("Failed to open the semaphore to read the lines from the server");
    return 1;
  }

  // Put file path on the shared memory
  strncpy(file_details->path, this->file_path, 255);

  // Let the server know that the file path is put on the shared memory
  sem_post(sem_barrier);

  // Append lines received from the server to the file_lines vector.
  // We repeat this loop until we reach the EOT which means the transmission is
  // done.
  while (file_lines.empty() ||
         file_lines.back().find(EOT) == std::string::npos) {
    // Wait for server to put a line in the shared memory
    sem_wait(sem_lines);

    file_lines.push_back(file_details->line);

    // Let the server know we are ready to read the next line
    sem_post(sem_barrier);
  }

  // Remove EOT from the vector
  // EOT is always the last string received so pop_back will remote EOT
  file_lines.pop_back();

  // Check whether the last line (before EOT) is INVALID_FILE
  if (file_lines.back() == INVALID_FILE) {
    std::cerr << "INVALID FILE" << std::endl;
  } else {
    // Create a vector of 4 threads for searching
    std::vector<pthread_t> threads(NUM_THREADS);

    // We pass each thread a number of args of type searchThreadStruct which
    // includes:
    // - start index
    // - end index
    // - Context = this instance
    std::vector<searchThreadStruct> thread_args(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
      thread_args[i].start_idx = i * (file_lines.size() / NUM_THREADS);
      if (i != NUM_THREADS - 1) {
        thread_args[i].end_idx = (i + 1) * (file_lines.size() / NUM_THREADS);
      } else {
        thread_args[i].end_idx = file_lines.size();
      }

      // Allows the threads to access properties of class instance
      thread_args[i].context = this;

      pthread_create(&threads[i], nullptr, search_thread,
                     static_cast<void*>(&thread_args[i]));
    }

    // Wait for all threads to finish
    for (auto thread : threads) {
      pthread_join(thread, nullptr);
    }

    // Print the results
    int counter = 0;
    for (auto match : matches) {
      std::cout << ++counter << "\t" << match << std::endl;
    }
  }

  // Unmap file_details which was attached to the created shared memory
  munmap(file_details, shm_size);

  // close the shared memory object
  close(shm_fd);

  // remove the shared memory
  shm_unlink(&shm_name[0]);

  // Destroy the threads semaphore created by the client
  sem_destroy(sem_threads);

  return 0;
}

void* CsvClient::search_thread(void* arg) {
  searchThreadStruct args = *static_cast<searchThreadStruct*>(arg);

  // Vector of search expressions
  std::vector<std::string> search_expressions =
      args.context->search_expressions;

  // Vector of all file lines
  std::vector<std::string> file_lines = args.context->file_lines;

  for (int i = args.start_idx; i < args.end_idx; i++) {
    for (std::vector<std::string>::iterator t = search_expressions.begin();
         t != search_expressions.end(); ++t) {
      if (file_lines[i].find(*t) != std::string::npos) {
        // Here we use a semaphore to manage access to the matches vector, so
        // the threads won't modify the vector at the same time which causes
        // errors
        sem_wait(args.context->sem_threads);
        args.context->matches.push_back(file_lines[i]);
        sem_post(args.context->sem_threads);
      }
    }
  }
  return nullptr;
}
