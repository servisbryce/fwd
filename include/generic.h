#ifndef GENERIC_H_
#define GENERIC_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "cli.h"

/* An interface for an unencrypted generic T.C.P. proxied connection. */
int unprotected_generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, char *hostname, bool protected_downstream, int sockfd, int timeout);

/* An interface for an encrypted generic T.C.P. proxied connection. */
int protected_generic_interface(SSL_CTX *upstream_context, struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, char *hostname, bool protected_downstream, int sockfd, int timeout);

#endif