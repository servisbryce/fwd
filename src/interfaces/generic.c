#include "../../include/cli.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* An interface for an unencrypted generic T.C.P. proxied connection. */
int unprotected_generic_interface(struct sockaddr *downstream_sockaddr, int sockfd) {

    /* Ensure our parameters are valid. */
    if (sockfd < 0) {

        return -1;

    }

    /* We should keep this infinite loop running forever as we're acting as a server. */
    while (1) {

        /* Prepare to store data regarding our new client. */
        int client_sockfd;

        /* Accept our new connection regarding our client. */
        if ((client_sockfd = accept(sockfd, NULL, NULL)) < 0) {

            fprintf(stderr, "There was an error while trying to accept an incoming connection!\n");
            continue;

        }

        /* Fork our process. */
        int child_pid;
        if ((child_pid = fork()) < 0) {

            fprintf(stderr, "There was an error while trying to fork the main process!\n");
            continue;

        }

        /* If we're the main process, we should move on while letting the child process run through it's course. */
        if (child_pid != 0) {

            /* For weird socket reasons, we should probably close this on our end so that the client can use the socket properly. */
            close(client_sockfd);
            continue;

        }

        /* Setup our pipes before we split, yet again. */
        int downstream_to_child_fd[2];
        int child_to_downstream_fd[2];
        
        if (pipe(downstream_to_child_fd) == -1) {

            fprintf(stderr, "There was an error while trying to construct a pipe between the downstream and child processes.\n");

        }

        if (pipe(child_to_downstream_fd) == -1) {

            fprintf(stderr, "There was an error while trying to construct a pipe between the child and downstream processes.\n");

        }

        /* Then, we'll fork again for the downstream connection handler. */
        int downstream_child_pid;
        if ((downstream_child_pid = fork()) < 0) {

            fprintf(stderr, "There was an error while trying to fork the child process!\n");
            continue;

        }

        /* We're the child process. */
        if (downstream_child_pid != 0) {

            /* Close the writing end as the downstream process will be writing to the child process. */
            close(downstream_to_child_fd[1]);

            /* Close the reading end as we'll be writing to the child process. */
            close(child_to_downstream_fd[0]);

            /* Setup our epoll. */
            struct epoll_event event, events[3];
            memset(&event, 0, sizeof(event));
            memset(&events, 0, sizeof(events));
            int epoll_fd;
            if ((epoll_fd = epoll_create1(0)) < 0) {

                fprintf(stderr, "There was an error while trying to create an epoll file descriptor!\n");
                exit(EXIT_FAILURE);

            }

            /* Downstream to child reading socket poll. */
            event.events = EPOLLIN;
            event.data.fd = downstream_to_child_fd[0];
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, downstream_to_child_fd[0], &event);

            /* Client to upstream reading socket poll. */
            event.events = EPOLLIN;
            event.data.fd = client_sockfd;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event);

            while (1) {

                int n = epoll_wait(epoll_fd, events, 3, -1);
                for (int i = 0; i < n; i++) {

                    /* Handle sockets that have been written to. */
                    if (events[i].events & EPOLLIN) {

                        int fd = events[i].data.fd;
                        /* Something has been written to us from the downstream server. */
                        if (fd == downstream_to_child_fd[0]) {

                            /* Read the message from the downstream server. */
                            ssize_t downstream_message_length = 8000000;
                            char *downstream_message = (char *) malloc(downstream_message_length);
                            if ((downstream_message_length = read(downstream_to_child_fd[0], downstream_message, downstream_message_length)) < 0) {

                                fprintf(stderr, "There was an issue while trying to read the downstream message!\n");
                                free(downstream_message);
                                close(downstream_to_child_fd[0]);
                                close(child_to_downstream_fd[1]);
                                close(client_sockfd);
                                break;

                            }

                            /* Reduce excessive memory usage by unallocating unused bytes. */
                            downstream_message = (char *) realloc(downstream_message, downstream_message_length);

                            /* Write the downstream message to the upstream client. */
                            if (write(client_sockfd, downstream_message, downstream_message_length) < 0) {

                                fprintf(stderr, "There was an issue while trying to write the downstream message to the upstream client!\n");
                                free(downstream_message);
                                close(downstream_to_child_fd[0]);
                                close(child_to_downstream_fd[1]);
                                close(client_sockfd);
                                break;

                            }

                            /* Free the downstream message as we're done with it. */
                            free(downstream_message);

                        /* Something has been written to us from the upstream client. */
                        } else if (fd == client_sockfd) {

                            /* Read the message from the upstream client. */
                            ssize_t upstream_message_length = 8000000;
                            char *upstream_message = (char *) malloc(upstream_message_length);
                            if ((upstream_message_length = read(client_sockfd, upstream_message, upstream_message_length)) < 0) {

                                fprintf(stderr, "There was an issue while trying to read the downstream message!\n");
                                free(upstream_message);
                                close(downstream_to_child_fd[0]);
                                close(child_to_downstream_fd[1]);
                                close(client_sockfd);
                                break;

                            }

                            /* Reduce excessive memory usage by unallocating unused bytes. */
                            upstream_message = (char *) realloc(upstream_message, upstream_message_length);

                            /* Write the upstream message to the downstream server. */
                            if (write(child_to_downstream_fd[1], upstream_message, upstream_message_length) < 0) {

                                fprintf(stderr, "There was an issue while trying to write the downstream message to the upstream client!\n");
                                free(upstream_message);
                                close(downstream_to_child_fd[0]);
                                close(child_to_downstream_fd[1]);
                                close(client_sockfd);
                                break;

                            }

                            /* Free the upstream_message message as we're done with it. */
                            free(upstream_message);

                        }

                    }

                }

            }

        /* We're the downstream child process. */
        } else {

            /* Close the reading end as the downstream process will be writing to the child process. */
            close(downstream_to_child_fd[0]);

            /* Close the writing end as the child process will be writing to us. */
            close(child_to_downstream_fd[1]);

            /* Yeah, we also don't really want to have an open upstream socket in the downstream child process either. */
            close(client_sockfd);

            /* Determine our downstream structure length. */
            size_t downstream_sockaddr_length = sizeof(struct sockaddr_in6);
            if (downstream_sockaddr->sa_family == AF_INET) {

                downstream_sockaddr_length = sizeof(struct sockaddr_in);

            }

            /* Now, we'll connect to our downstream server since the upstream connection has been created. */
            int downstream_sockfd = socket(downstream_sockaddr->sa_family, SOCK_STREAM, 0);
            if (connect(downstream_sockfd, downstream_sockaddr, downstream_sockaddr_length) < 0) {

                fprintf(stderr, "There was an error while trying to connect to the downstream server!\n");
                close(child_to_downstream_fd[0]);
                close(downstream_to_child_fd[1]);
                close(downstream_sockfd);
                exit(EXIT_FAILURE);

            }

            /* Setup our epoll. */
            struct epoll_event event, events[3];
            memset(&event, 0, sizeof(event));
            memset(&events, 0, sizeof(events));
            int epoll_fd;
            if ((epoll_fd = epoll_create1(0)) < 0) {

                fprintf(stderr, "There was an error while trying to create an epoll file descriptor!\n");
                exit(EXIT_FAILURE);

            }

            /* Downstream to child reading socket poll. */
            event.events = EPOLLIN;
            event.data.fd = child_to_downstream_fd[0];
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, child_to_downstream_fd[0], &event);

            /* Client to upstream reading socket poll. */
            event.events = EPOLLIN;
            event.data.fd = downstream_sockfd;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, downstream_sockfd, &event);

            while (1) {

                int n = epoll_wait(epoll_fd, events, 3, -1);
                for (int i = 0; i < n; i++) {

                    /* Handle sockets that have been written to. */
                    if (events[i].events & EPOLLIN) {

                        int fd = events[i].data.fd;
                        /* Something has been written to us from the upstream client. */
                        if (fd == child_to_downstream_fd[0]) {

                            /* Read the message from the upstream client. */
                            ssize_t upstream_message_length = 8000000;
                            char *upstream_message = (char *) malloc(upstream_message_length);
                            if ((upstream_message_length = read(child_to_downstream_fd[0], upstream_message, upstream_message_length)) < 0) {

                                fprintf(stderr, "There was an issue while trying to read the upstream message!\n");
                                free(upstream_message);
                                close(child_to_downstream_fd[0]);
                                close(downstream_to_child_fd[1]);
                                close(downstream_sockfd);
                                break;

                            }

                            /* Reduce excessive memory usage by unallocating unused bytes. */
                            upstream_message = (char *) realloc(upstream_message, upstream_message_length);

                            /* Write the downstream message to the upstream client. */
                            if (write(downstream_sockfd, upstream_message, upstream_message_length) < 0) {

                                fprintf(stderr, "There was an issue while trying to write the upstream message to the downstream server!\n");
                                free(upstream_message);
                                close(child_to_downstream_fd[0]);
                                close(downstream_to_child_fd[1]);
                                close(downstream_sockfd);
                                break;

                            }

                            /* Free the downstream message as we're done with it. */
                            free(upstream_message);

                        /* Something has been written to us from the downstream server. */
                        } else if (fd == downstream_sockfd) {

                            /* Read the message from the downstream server. */
                            ssize_t downstream_message_length = 8000000;
                            char *downstream_message = (char *) malloc(downstream_message_length);
                            if ((downstream_message_length = read(downstream_sockfd, downstream_message, downstream_message_length)) < 0) {

                                fprintf(stderr, "There was an issue while trying to read the downstream message!\n");
                                free(downstream_message);
                                close(child_to_downstream_fd[0]);
                                close(downstream_to_child_fd[1]);
                                close(downstream_sockfd);
                                break;

                            }

                            /* Reduce excessive memory usage by unallocating unused bytes. */
                            downstream_message = (char *) realloc(downstream_message, downstream_message_length);

                            /* Write the upstream message to the downstream server. */
                            if (write(downstream_to_child_fd[1], downstream_message, downstream_message_length) < 0) {

                                fprintf(stderr, "There was an issue while trying to write the downstream message to the upstream client!\n");
                                free(downstream_message);
                                close(child_to_downstream_fd[0]);
                                close(downstream_to_child_fd[1]);
                                close(downstream_sockfd);
                                break;

                            }

                            /* Free the downstream message message as we're done with it. */
                            free(downstream_message);

                        }

                    }

                }

            }

        }
        
        /* Ensure that any child process cannot gain it's own independence and receive connections on behalf of the main process. */
        exit(EXIT_SUCCESS);

    }

}