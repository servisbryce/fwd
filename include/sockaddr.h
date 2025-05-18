#ifndef SOCKADDR_H_
#define SOCKADDR_H_

#include <stdint.h>

/* Convert our port and address into a valid socket address structure. */
struct sockaddr *construct_sockaddr(char *address, uint16_t port);

#endif