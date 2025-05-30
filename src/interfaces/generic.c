#include "../../include/cli.h"
#include "../../include/tls.h"
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* An interface for an unencrypted generic T.C.P. proxied connection. */

int generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, SSL_CTX *upstream_context, char *downstream_hostname, bool downstream_protected, int timeout, int sockfd) {



}