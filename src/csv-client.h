// Author: Rasoul Nikmehr

#ifndef CSV_CLIENT_H
#define CSV_CLIENT_H

#include <string>
#include <vector>
#include <semaphore.h>

class CsvClient {
 public:
  CsvClient(const char* file_path, const std::vector<std::string> search_expressions);

  int execute();
  static void* search_thread(void* arg);
  std::vector<std::string> matches;
  std::vector<std::string> file_lines;

 private:
  const char* file_path;
  std::vector<std::string> search_expressions;
  sem_t* sem_threads;
};

struct searchThreadStruct {
  int start_idx;
  int end_idx;
  CsvClient* context;
};

#endif