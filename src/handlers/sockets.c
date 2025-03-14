#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

void handle_sockaddr_in(char *address, uint16_t port, struct sockaddr_in *sockaddr) {

    /* Construct the socket address metadata. */
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons(port);

    /* Convert the sockets address from the human readable format to binary. */
    if (inet_pton(AF_INET, address, &sockaddr->sin_addr) != 1) {

        fprintf(stderr, "The address provided is malformed.");
        exit(EXIT_FAILURE);

    }

}