#ifndef GENERIC_H_
#define GENERIC_H_

#include "cli.h"

/* An interface for an unencrypted generic T.C.P. proxied connection. */
int unprotected_generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, int sockfd, int timeout);

/* An interface for an encrypted generic T.C.P. proxied connection. */
int protected_generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, char *certificate_path, char *certificate_key_path, int sockfd, int timeout);

#endif