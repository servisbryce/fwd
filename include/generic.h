#ifndef GENERIC_H_
#define GENERIC_H_

#include "cli.h"

int unprotected_generic_interface(struct sockaddr *downstream_sockaddr, int sockfd);

#endif