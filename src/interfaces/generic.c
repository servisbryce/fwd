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

/* Create a function to automatically read from a socket and store it in a buffer. */
void *unprotected_receive(int sockfd, ssize_t *buffer_length) {

    /* Ensure our parameters are valid. */
    if (sockfd < 0 || !buffer_length) {

        return NULL;

    }

    /* Write to our buffer from whatever data we've received through the socket. */
    char *buffer = (char *) malloc(*buffer_length);
    if ((*buffer_length = read(sockfd, buffer, *buffer_length)) <= 0) {

        free(buffer);
        return NULL;

    }

    /* We should also trim the buffer to make sure that we aren't overallocating chunks of memory when we don't need that much. */
    buffer = (char *) realloc(buffer, *buffer_length);
    return buffer;

}

/* Create a function to automatically write to a socket from a buffer. */
int unprotected_transmit(int sockfd, void *buffer, ssize_t buffer_length) {

    /* Ensure our parameters are valid. */
    if (sockfd < 0 || !buffer) {

        return -1;

    }

    /* Write from the buffer through the socket. */
    ssize_t written_length;
    if ((written_length = write(sockfd, buffer, buffer_length)) <= 0) {

        return -1;

    }

    return 0;

}

/* Create a function to automatically write to a socket from a buffer securely. */
int protected_transmit(SSL *ssl, void *buffer, ssize_t buffer_length) {

    /* Ensure our parameters are valid. */
    if (!ssl || !buffer) {

        return -1;

    }

    /* Write from the buffer through the socket. */
    if (SSL_write(ssl, buffer, buffer_length) <= 0) {

        return -1;

    }

    return 0;


}

/* Create a function to automatically read from a socket to a buffer securely. */
void *protected_receive(SSL *ssl, size_t *buffer_length) {

    /* Ensure our parameters are valid. */
    if (!ssl || !buffer_length) {

        return NULL;

    }

    char *buffer = (char *) malloc(*buffer_length);
    if (SSL_read_ex(ssl, buffer, *buffer_length, buffer_length) <= 0) {

        free(buffer);
        return NULL;

    }

    buffer = (char *) realloc(buffer, *buffer_length);
    return buffer;

}

/* A generic interface for handling connections where a protocol-specific enhancement isn't specified. */
int generic_interface(struct sockaddr *downstream_sockaddr, struct sockaddr *upstream_sockaddr, SSL_CTX *upstream_context, char *downstream_hostname, bool downstream_protected, int timeout, int sockfd) {

    /* Ensure our parameters are valid. */
    if (!downstream_protected || !upstream_sockaddr || !upstream_context || !downstream_hostname || sockfd < 0) {

        return -1;

    }

    /* Determine our structure lengths. */
    size_t downstream_sockaddr_length = sizeof(struct sockaddr_in6);
    if (downstream_sockaddr->sa_family == AF_INET) {

        downstream_sockaddr_length = sizeof(struct sockaddr_in);

    }

    size_t upstream_sockaddr_length = sizeof(struct sockaddr_in6);
    if (upstream_sockaddr->sa_family == AF_INET) {

        upstream_sockaddr_length = sizeof(struct sockaddr_in);

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

        /* Connect to the downstream server. */
        if (connect(downstream_sockfd, downstream_sockaddr, downstream_sockaddr_length) < 0) {

            if (tls_client_context) {

                SSL_CTX_free(tls_client_context);

            }

            close(downstream_to_upstream_pipe[1]);
            close(upstream_to_downstream_pipe[0]);
            close(downstream_sockfd);
            return -1;

        }

        /* Perform our S.S.L. handshake if needed. */
        SSL *ssl = NULL;
        if (tls_client_context) {

            if (!(ssl = SSL_new(tls_client_context))) {

                SSL_CTX_free(tls_client_context);
                close(downstream_to_upstream_pipe[1]);
                close(upstream_to_downstream_pipe[0]);
                close(downstream_sockfd);
                return -1;

            }

            SSL_set_fd(ssl, downstream_sockfd);
            if (SSL_connect(ssl) <= 0) {

                SSL_free(ssl);
                SSL_CTX_free(tls_client_context);
                close(downstream_to_upstream_pipe[1]);
                close(upstream_to_downstream_pipe[0]);
                close(downstream_sockfd);
                return -1;

            }

        }
        
        /* Setup our multiplexed socket monitor. */
        int epollfd;
        if ((epollfd = epoll_create1(0)) < 0) {

            if (tls_client_context) {

                SSL_free(ssl);
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

                SSL_free(ssl);
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

                SSL_free(ssl);
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
        memset(&epoll_events, 0, sizeof(epoll_events));
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

                    /* Receive the upstream message. */
                    ssize_t upstream_message_length = 8000000;
                    void *upstream_message = NULL;
                    if (!(upstream_message = unprotected_receive(upstream_to_downstream_pipe[0], &upstream_message_length))) {

                        SSL_free(ssl);
                        SSL_CTX_free(tls_client_context);
                        close(downstream_to_upstream_pipe[1]);
                        close(upstream_to_downstream_pipe[0]);
                        close(downstream_sockfd);
                        close(epollfd);
                        return -1;

                    }

                    /* Write our message to the downstream server. */
                    if (ssl) {
    
                        if (protected_transmit(ssl, upstream_message, upstream_message_length) < 0) {

                            free(upstream_message);
                            SSL_free(ssl);
                            SSL_CTX_free(tls_client_context);
                            close(downstream_to_upstream_pipe[1]);
                            close(upstream_to_downstream_pipe[0]);
                            close(downstream_sockfd);
                            close(epollfd);
                            return -1;

                        }

                        free(upstream_message);

                    } else {

                        /* Transmit our message. If we failed, then we should break down and kill ourselves. */
                        if (unprotected_transmit(downstream_sockfd, upstream_message, upstream_message_length) < 0) {

                            free(upstream_message);
                            close(downstream_to_upstream_pipe[1]);
                            close(upstream_to_downstream_pipe[0]);
                            close(downstream_sockfd);
                            close(epollfd);
                            return -1;

                        }

                        free(upstream_message);

                    }

                /* We've received a message from the downstream server! */
                } else if (epoll_events[i].data.fd == downstream_sockfd) {

                    void *downstream_message = NULL;
                    ssize_t downstream_message_length = 8000000;
                    if (ssl) {

                        /* Receive the message if we're using T.L.S. */
                        size_t unsigned_buffer_length = (size_t) downstream_message_length;
                        if (!(downstream_message = protected_receive(ssl, &unsigned_buffer_length))) {

                            free(downstream_message);
                            SSL_free(ssl);
                            SSL_CTX_free(tls_client_context);
                            close(downstream_to_upstream_pipe[1]);
                            close(upstream_to_downstream_pipe[0]);
                            close(downstream_sockfd);
                            close(epollfd);
                            return -1;

                        }

                    } else {

                        /* Otherwise, just read the raw data. */
                        if (!(downstream_message = unprotected_receive(downstream_sockfd, &downstream_message_length))) {

                            free(downstream_message);
                            close(downstream_to_upstream_pipe[1]);
                            close(upstream_to_downstream_pipe[0]);
                            close(downstream_sockfd);
                            close(epollfd);
                            return -1;

                        }

                    }

                    /* Transmit the data upstream. */
                    if (unprotected_transmit(downstream_to_upstream_pipe[1], downstream_message, downstream_message_length) < 0) {

                        if (ssl) {

                            SSL_free(ssl);
                            SSL_CTX_free(tls_client_context);

                        }

                        free(downstream_message);
                        close(downstream_to_upstream_pipe[1]);
                        close(upstream_to_downstream_pipe[0]);
                        close(downstream_sockfd);
                        close(epollfd);
                        return -1;

                    }
                    
                    free(downstream_message);

                }

            }

        }

    }

    /* Return success. */
    return 0;

}