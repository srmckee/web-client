// Copyright
//
// Spencer McKee
// 1841180
// srmckee@cs.washington.edu

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

using namespace std;

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
