#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void handle_sockaddr_in(char *address, uint16_t port, struct sockaddr_in *sockaddr) {

    /* Construct the socket address metadata. */
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons(port);

    /* Convert the sockets address from the human readable format to binary. */
    if (inet_pton(AF_INET, address, &sockaddr->sin_addr) != 1) {

        fprintf(stderr, "There was an unexpected error while converting the provided address: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

}

int handle_socket(struct sockaddr_in *sockaddr) {

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        fprintf(stderr, "There was an unexpected error while constructing a socket: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {

        fprintf(stderr, "There was an unexpected error while configuring a socket: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (bind(sock, (struct sockaddr*) sockaddr, sizeof(struct sockaddr)) < 0) {

        fprintf("There was an unexpected error while binding a socket: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (listen(sock, 4096) < 0) {

        fprintf("There was an unexpected error while listening on a socket: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    return sock;

}