// Author: Rasoul Nikmehr

#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>


// Prints the given message to stderr
void print_err(const std::string message);

void log(const std::string message);

// Name of the semaphore used to notify the server that client is ready to receive lines
extern const char* SEM_BARRIER_NAME;
// Name of the semaphore used to notify the client that server has put a line on the shared memory
extern const char* SEM_LINES_NAME;
// Name of the shared memory which is used to transmit data between client and server
extern const char* shm_name;

// Specifies end of transmission
extern const char* EOT;
// Specifies that the file path put on the shared memory (by the client) is not valid
extern const char* INVALID_FILE;

// This structure is used to transmit data between the client and the server
struct fileDetails {
  static const std::size_t path_length = 255;
  char path[path_length];
  static const std::size_t line_length = 512;
  char line[line_length];
};

#endif