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

    /* Allocate our pipes on the stack. */
    int downstream_to_upstream_pipe[2];
    int upstream_to_downstream_pipe[2];

    /* Create our pipes. */
    if (pipe(downstream_to_upstream_pipe) < 0) {

        return -1;

    }

    if (pipe(upstream_to_downstream_pipe) < 0) {

        return -1;

    }

    /* Branch off into our respective processes. */
    int process_id;
    if ((process_id = fork()) == 0) {

        /* We're the forked process (upstream-facing process). */

        /* Close the write-end of the downstream-to-upstream pipe as we'll be receiving messages to process through this pipe. */
        close(downstream_to_upstream_pipe[1]);

        /* Close the read-end of the upstream-to-downstream pipe as we'll be writing messages to process through this pipe. */
        close(upstream_to_downstream_pipe[0]);



    } else {

        /* We're the parent (downstream-facing process). */

        /* Close the read-end of the downstream-to-upstream pipe as we'll be writing messages to process through this pipe. */
        close(downstream_to_upstream_pipe[0]);

        /* Close the write-end of the upstream-to-downstream pipe as we'll be reading messages to process through this pipe. */
        close(upstream_to_downstream_pipe[1]);

    }

    /* Return success. */
    return 0;

}