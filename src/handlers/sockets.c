#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

struct sockaddr_in *handle_sockaddr_in(char *address, uint16_t port) {

    /* Construct the socket address from hostname or address data. */
    struct addrinfo hints, *result;
    struct sockaddr_in *sockaddr;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    if (getaddrinfo(address, NULL, &hints, &result) != 0) {

        fprintf(stderr, "There was an unexpected error while converting the provided address or hostname to a machine-readable format: '%s'\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    /* File out the rest of the socket address structure. */
    sockaddr = (struct sockaddr_in *) result->ai_addr;
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons(port);
    freeaddrinfo(result);
    return sockaddr;

}

int handle_socket(struct sockaddr_in *sockaddr) {

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        fprintf(stderr, "There was an unexpected error while constructing a socket: '%s'.\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {

        fprintf(stderr, "There was an unexpected error while configuring a socket: '%s'.\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (bind(sock, (struct sockaddr*) sockaddr, sizeof(struct sockaddr)) < 0) {

        fprintf(stderr, "There was an unexpected error while binding a socket: '%s'.\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (listen(sock, 4096) < 0) {

        fprintf(stderr, "There was an unexpected error while listening on a socket: '%s'.\n", strerror(errno));
        exit(EXIT_FAILURE);

    }

    return sock;

}