#ifndef GENERIC_H_
#define GENERIC_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "cli.h"

int generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, SSL_CTX *upstream_context, char *downstream_hostname, bool downstream_protected, int timeout, int sockfd);

#endif