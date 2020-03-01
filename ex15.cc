// Copyright
//
// Spencer McKee
// 1841180
// srmckee@cs.washington.edu

using namespace std;

#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <netdb.h>
#include <assert.h>
#include <iostream>

#include "./network.h"

#define BUF 128

// Prints the usage of the program
void Usage(char *progname);

// Accepts a hostname, port, and filename
// Reads the contents of filename and writes to server of
// given hostname and port
// returns EXIT_SUCCESS for success and EXIT_FAILURE otherwise
int main(int argc, char** argv) {
  if (argc != 4) {
    Usage(argv[0]);
  }

  char* hostname = argv[1];
  unsigned short port = (unsigned short) atoi(argv[2]);
  char* filename = argv[3];

  // Resolve hostname and get port
  struct sockaddr_storage addr;
  size_t addrlen;
  if (!LookupName(hostname, port, &addr, &addrlen)) {
    Usage(argv[0]);
  }
  // Create socket and connect
  int socket_fd;
  if (!Connect(addr, addrlen, &socket_fd)) {
    Usage(argv[0]);
  }
  // Read from file
  char readbuf[BUF];
  int res;
  // Open file to read
  int32_t f = open(filename, O_RDONLY);
  if (f == -1) {
    perror("open failed");
    return EXIT_FAILURE;
  }
  // read file
  struct stat st;
  stat(filename, &st);
  int file_size = st.st_size;
  int bytes_left = file_size;
  while (bytes_left > 0) {
    res = read(f, readbuf + (file_size - bytes_left), bytes_left);
    if (res == -1) {
      if (errno != EINTR) {
        perror("read failed");
        return EXIT_FAILURE;
      }
    } else if (res == 0) {
      fprintf(stderr, "end of file reached: %" PRId32
          " bytes read.\n", file_size - bytes_left);
      readbuf[file_size - bytes_left] = '\0';
    }
    bytes_left -= res;
    // write to server
    int wres = write(socket_fd, readbuf, res);
    if (wres == 0) {
      cerr << "socket closed prematurely" << endl;
      close(socket_fd);
      return EXIT_FAILURE;
    }
    if (wres == -1) {
      if (errno == EINTR)
        continue;
      cerr << "socket write failure: " << strerror(errno) << std::endl;
      close(socket_fd);
      return EXIT_FAILURE;
    }
  }
  // close file
  close(f);
  // close connection
  close(socket_fd);
  return EXIT_SUCCESS;
}

void Usage(char *progname) {
  cerr << "usage: " << progname << " hostname port filename" << endl;
  exit(EXIT_FAILURE);
}
/*
bool LookupName(char *name, unsigned short port,
    struct sockaddr_storage *ret_addr, size_t *ret_addrlen) {
  struct addrinfo hints, *results;
  int retval;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((retval = getaddrinfo(name, nullptr, &hints, &results)) != 0) {
    cerr << "getaddrinfo failed: ";
    cerr << gai_strerror(retval) << endl;
    return false;
  }

  if (results->ai_family == AF_INET) {
    struct sockaddr_in *v4addr =
       reinterpret_cast<struct sockaddr_in *>(results->ai_addr);
    v4addr->sin_port = htons(port);
  } else if (results->ai_family == AF_INET6) {
    struct sockaddr_in6 *v6addr =
      reinterpret_cast<struct sockaddr_in6 *>(results->ai_addr);
    v6addr->sin6_port = htons(port);
  } else {
    cerr << "getaddrinfo failed to provide an IPv4 or IPv6 address";
    cerr << endl;
  freeaddrinfo(results);
  return false;
  }

  assert(results != nullptr);
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  *ret_addrlen = results->ai_addrlen;

  freeaddrinfo(results);
  return true;
}

bool Connect(const struct sockaddr_storage &addr,
    const size_t &addrlen,
    int *ret_fd) {
  int socket_fd = socket(addr.ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    cerr << "socket() failed: " << strerror(errno) << endl;
    return false;
  }

  int res = connect(socket_fd,
      reinterpret_cast<const sockaddr *>(&addr),
      addrlen);
  if (res == -1) {
    cerr << "connect() failed: " << strerror(errno) << endl;
    return false;
  }

  *ret_fd = socket_fd;
  return true;
}
*/
