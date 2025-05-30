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

        /* Create our T.L.S. client context. We'll rely on the downstream protected variable to determine if the downstream supports T.L.S. However, we'll use the context itself to determine this to prevent errors later. */
        SSL_CTX *tls_client_context = NULL;
        if (downstream_protected) {

            tls_client_context = create_client_context();

        }

        /* Create our downstream socket. */
        int downstream_sockfd;
        if ((downstream_sockfd = socket(downstream_sockaddr->sa_family, SOCK_STREAM, 0)) < 0) {

            if (tls_client_context) {

                SSL_CTX_free(tls_client_context);

            }

            close(downstream_to_upstream_pipe[1]);
            close(upstream_to_downstream_pipe[0]);
            return -1;
            
        } 

        /* Setup our multiplexed socket monitor. */
        int epollfd;
        if ((epollfd = epoll_create1(0)) < 0) {

            if (tls_client_context) {

                SSL_CTX_free(tls_client_context);

            }

            close(downstream_to_upstream_pipe[1]);
            close(upstream_to_downstream_pipe[0]);
            close(downstream_sockfd);
            return -1;

        }

        /* Create our events and clean them as they're allocated on the stack. */
        struct epoll_event upstream_to_downstream_pipe_event, downstream_sockfd_event;
        memset(&upstream_to_downstream_pipe_event, 0, sizeof(upstream_to_downstream_pipe_event));
        memset(&downstream_sockfd, 0, sizeof(downstream_sockfd));

        /* Setup our events. */
        upstream_to_downstream_pipe_event.events = EPOLLIN;
        upstream_to_downstream_pipe_event.data.fd = upstream_to_downstream_pipe[0];
        downstream_sockfd_event.events = EPOLLIN;
        downstream_sockfd_event.data.fd = downstream_sockfd;
        
        /* Attach our sockets to the polling mechanism. */
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, upstream_to_downstream_pipe[0], &upstream_to_downstream_pipe_event) == -1) {

            if (tls_client_context) {

                SSL_CTX_free(tls_client_context);

            }

            close(downstream_to_upstream_pipe[1]);
            close(upstream_to_downstream_pipe[0]);
            close(downstream_sockfd);
            close(epollfd);
            return -1;

        }

        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, downstream_sockfd, &downstream_sockfd_event) == -1) {

            if (tls_client_context) {

                SSL_CTX_free(tls_client_context);

            }

            close(downstream_to_upstream_pipe[1]);
            close(upstream_to_downstream_pipe[0]);
            close(downstream_sockfd);
            close(epollfd);
            return -1;

        }

        /* Check our socket multiplexer for events. */
        struct epoll_event epoll_events[2];
        memset(&epoll_event, 0, sizeof(epoll_events));
        while (1) {

            /* Wait for events. */
            int events;
            if ((events = epoll_wait(epollfd, epoll_events, 2, -1)) < 0) {

                break;

            }

            /* Iterate through our events. */
            for (int i = 0; i < events; i++) {

                /* We've received a message from the upstream process! */
                if (epoll_events[i].data.fd == upstream_to_downstream_pipe[0]) {



                /* We've received a message from the downstream server! */
                } else if (epoll_events[i].data.fd == downstream_sockfd) {



                }

            }

        }

    }

    /* Return success. */
    return 0;

}