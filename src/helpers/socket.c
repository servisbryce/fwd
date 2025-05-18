#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Create a socket from a socket address structure. */
int create_socket(struct sockaddr *sockaddr, int timeout) {

    /* Ensure that our parameters actually exist. */
    if (!sockaddr) {

        return -1;

    }

    /* Create our socket. */
    int sockfd;
    if ((sockfd = socket(sockaddr->sa_family, SOCK_STREAM, 0)) < 0) {

        fprintf(stderr, "There was an error while trying to create a socket!\n");
        exit(EXIT_FAILURE);

    }

    /* Set our socket options. */
    struct timeval socket_timeout;
    memset(&socket_timeout, 0, sizeof(socket_timeout));
    socket_timeout.tv_sec = timeout;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &socket_timeout, sizeof(socket_timeout)) < 0) {

        fprintf(stderr, "There was an error while trying to set the socket timeout!\n");
        exit(EXIT_FAILURE);

    }

    int option = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) < 0) {

        fprintf(stderr, "There was an error while trying to set socket options!\n");
        exit(EXIT_FAILURE);

    }

    /* Determine our socket address length. */
    size_t sockaddr_length = sizeof(struct sockaddr_in6);
    if (sockaddr->sa_family == AF_INET) {

        sockaddr_length = sizeof(struct sockaddr_in);

    }

    /* Bind to our socket. */
    if (bind(sockfd, sockaddr, sockaddr_length) < 0) {

        fprintf(stderr, "There was an error while trying to bind to our socket!\n");
        exit(EXIT_FAILURE);

    }

    /* Listen to our socket. */
    if (listen(sockfd, 4096) < 0) {

        fprintf(stderr, "There was an error while trying to listen to our socket!\n");
        exit(EXIT_FAILURE);

    }

    /* Return our socket.*/
    return sockfd;

}