<<<<<<< HEAD
# Project3-IPC
A Client/Server project using Unix Domain Sockets for CSV file processing and inter-process communication as part of the Operating Systems coursework

# CSV Search Client/Server Application

## Build
Build the client and server with:
```
make csv-server
make csv-client
```
## Usage

### Server
Run the following command without any arguments
```
./csv-server
```

### Client
Run the following command by specifying the path to the csv file and search expression which can be a boolean expression using the "+" character. File path should be relative to the current directory which the program is running on.
```
./csv-client ./path/to/file.csv expr1 + expr2 + expr3 + ...
```
## Included files (inside the `src` directory)

### Common Header (`common.h`)

This file includes a number of constants which is common with the server and client:
- SEM_BARRIER_NAME: Name of the semaphore used by client and server that notifies server that the client has put the file path on the shared memory or when the client is ready to read the next line
- SEM_LINES_NAME: Name of the semaphore used to notify the client to read the line that is put on the shared memory by the server
- shm_name: name of the shared memory that is created and used by the client and also used by the server
- EOT: The server sends this at the end of each transmission (e.g. when all lines are transmitted via the shared memory or after sending INVALID_FILE to the client)
- INVALID_FILE: Is sent by the server to the client when the file path specified by the client is not correct.

The values of these constants and variables are defined in `common.cc`

### common.cc
This file includes definition of the methods and variables declared in `common.h`

### CSV Server (`csv-server.cc`)
The server code and its main method is implemented in this file.

### CSV Client Main file (`csv-client-main.cc`)
This file is the main entry to the client application. It checks the user input and if valid, creates an instance of the CsvClient class and starts the csv client.

### CSV Client Source (`csv-client.cc`)
This file defines the CsvClient class methods and includes the definition of the following methods:
- `int CsvClient::execute()` Main implementation of the csv client
- `void* CsvClient::search_thread(void* arg)` Thread that searches for the file contents at the given indexes

### CSV Client Header (`csv-client.h`)
This header file declares the CsvClient class and `SearchThreadStruct` struct. This struct is used to pass the data to the search threads created by the client.

### Makefile
Used to build the client and server and clean the files
>>>>>>> fc959ca (Initial commit for Project3-IPC)
