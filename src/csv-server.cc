// Author: Rasoul Nikmehr

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include <csignal>
#include <fstream>
#include <iostream>
#include <vector>

#include "common.h"

int main(int argc, char const* argv[]) {
  std::vector<std::string> file_lines;

  std::clog << "SERVER STARTED" << std::endl;

  // create the barrier semaphore
  mode_t sem_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
  sem_t* sem_barrier = sem_open(SEM_BARRIER_NAME, O_CREAT, sem_mode, 0);
  if (sem_barrier == SEM_FAILED) {
    print_err("Failed to open the barrier semaphore");
    return 1;
  }

  // The sem_lines semaphore is used to let the client know that a line is put
  // on the shared memory object
  sem_t* sem_lines = sem_open(SEM_LINES_NAME, O_CREAT, sem_mode, 0);
  if (sem_lines == SEM_FAILED) {
    print_err("Failed to open the second semaphore");
    return 1;
  }

  while (true) {
    // Start with an empty vector for each request
    file_lines.clear();

    // Wait for the client to put the file path on the shared memory
    sem_wait(sem_barrier);

    log("CLIENT REQUEST RECEIVED");

    // Size of the structure used to transmit data between client and server
    const int shm_size = sizeof(fileDetails);

    // Open the shared memory which is already created by the client
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
      print_err("Failed to open shared memory");
      return 1;
    }

    std::clog << "\tMEMORY OPEN" << std::endl;

    // NULL: We allow the kernel to select the suitable address for our mapping
    // PROT_READ and PROT_WRITE provide read and write access to the mapped
    // memory segment.
    // MAP_SHARED: Specifies that we want the mapped memory be
    // shared between multiple processes (client and server)
    fileDetails* file_details = static_cast<fileDetails*>(
        mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    if (file_details == MAP_FAILED) {
      print_err("Failed to map the memory");
      shm_unlink(shm_name);
    }

    std::clog << "\tOPENING: " << file_details->path << std::endl;

    // Get the file path from the shared memory
    std::ifstream file(file_details->path);

    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        // Add all lines of the file to the file_lines string vector
        file_lines.push_back(line);
      }
    } else {
      std::cerr << "Failed to open file " << file_details->path;
      // Let the client know that an invalid file path is sent
      file_lines.push_back(INVALID_FILE);
    }

    // At the end of the transmission we send EOT so the client knows that there
    // is nothing left to receive
    file_lines.push_back(EOT);

    // For each line of the file_lines vector, put that line on the shared
    // memory, let the client know that a new line is sent by the server and
    // wait until the client notifies us that it's ready to receive the next
    // line by the sem_barrier
    for (auto line : file_lines) {
      strncpy(file_details->line, &line[0], line.size() + 1);
      sem_post(sem_lines);
      sem_wait(sem_barrier);
    }

    // Close the file
    file.close();
    std::clog << "\tFILE CLOSED" << std::endl;

    // close shared memory
    close(shm_fd);
    std::clog << "\tMEMORY CLOSED" << std::endl;
  }

  return 0;
}
