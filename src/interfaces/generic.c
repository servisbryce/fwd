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

/* A generic interface for handling connections where a protocol-specific enhancement isn't specified. */
int generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, SSL_CTX *upstream_context, char *downstream_hostname, bool downstream_protected, int timeout, int sockfd) {

    /* Ensure our parameters are valid. */
    if (!downstream_protected || !upstream_sockaddr || !upstream_context || !downstream_hostname || sockfd < 0) {

        return -1;

    }

    

    /* Return success. */
    return 0;

}