// Author: Rasoul Nikmehr

#include "csv-client.h"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include "csv-client.h"

int main(int argc, char const *argv[])
{
  // Check whether at least a file path and a search expression is provided
  if (argc < 3) {
    std::cerr << "Usage: ./csv-client path/to/the/file.csv expr1 + expr2 + ..." << std::endl;
    return -1;
  }

  std::vector<std::string> search_expressions;
  for (int i = 2; i < argc; i++) {
    if (argv[i] != std::string("+")) {
      search_expressions.push_back(std::string(std::string(argv[i])));
    }
  }

  CsvClient client(argv[1], search_expressions);
  
  if (client.execute() < 0) {
    return 1;
  }
  
  return 0;
}
