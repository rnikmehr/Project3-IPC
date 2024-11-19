// Author: Rasoul Nikmehr

#include <iostream>
#include <string>
#include "common.h"

const char* SEM_BARRIER_NAME = "/barrier_sem2";
const char* shm_name = "/shared_memory1";
const char* SEM_LINES_NAME = "/lines_sem1";

const char* EOT = "END_OF_TRANSMISSION";
const char* INVALID_FILE = "INVALID_FILE";

void print_err(const std::string message) {
    std::cerr << message << " : " << strerror(errno) << std::endl;
}

void log(const std::string message) {
    std::clog << message << std::endl;
}