// Copyright
//
// Spencer McKee
// 1841180
// srmckee@cs.washington.edu

#ifndef _HOMES_IWS_SRMCKEE_EX15_NETWORK_H_
#define _HOMES_IWS_SRMCKEE_EX15_NETWORK_H_

// Looks up the ip address for the hostname
// Returns true if success and false otherwise
bool LookupName(char *name, unsigned short port,
  struct sockaddr_storage *ret_addr, size_t *ret_addrlen);

// Creates a socket and connects to the server at the given ip address
// Returns true if success and false otherwise
bool Connect(const struct sockaddr_storage &addr,
  const size_t &addrlen,
  int *ret_fd);

#endif  // _HOMES_IWS_SRMCKEE_EX15_NETWORK_H_
