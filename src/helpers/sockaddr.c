#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

/* Convert our port and address into a valid socket address structure. */
struct sockaddr *construct_sockaddr(char *address, uint16_t port) {

    /* Ensure our inputs are valid. */
    if (!address || port < 1) {

        return NULL;

    }

    /* Allocate, clean, and set our hints. */
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* Parse our address into a machine-readable format. */
    if (getaddrinfo(address, NULL, &hints, &result) != 0) {

        fprintf(stderr, "There was an error while trying to convert the human-readable address provided into a machine-readable address!\n");
        exit(EXIT_FAILURE);

    }

    /* Determine our socket address length. */
    size_t sockaddr_length = sizeof(struct sockaddr_in6);
    if (result->ai_family == AF_INET) {

        sockaddr_length = sizeof(struct sockaddr_in);

    }

    /* Copy our machine-readable address structure into a new one. */
    struct sockaddr *sockaddr = (struct sockaddr *) malloc(sockaddr_length);
    memcpy(sockaddr, result->ai_addr, sizeof(sockaddr_length));
    freeaddrinfo(result);

    /* Insert our port in a machine-readable format. */
    if (sockaddr_length == sizeof(struct sockaddr_in)) {

        struct sockaddr_in *sockaddr_in = (struct sockaddr_in *) sockaddr;
        sockaddr_in->sin_port = htons(port);

    } else {

        struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6 *) sockaddr;
        sockaddr_in6->sin6_port = htons(port);

    }

    /* Retun our completed structure. */
    return sockaddr;

}