#ifndef SOCKADDR_H_
#define SOCKADDR_H_

#include <stdint.h>

struct sockaddr *construct_sockaddr(char *address, uint16_t port);

#endif