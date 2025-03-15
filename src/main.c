#include "../include/arguments.h"
#include "../include/sockets.h"
#include "../include/tls.h"
#include <stdlib.h>

int main(int argc, char **argv) {

    arguments_t *arguments = handle_arguments(argc, argv);
    struct sockaddr_in *servaddr_in = handle_sockaddr_in(arguments->downstream->address, arguments->downstream->port);
    int servsock = handle_socket(servaddr_in);
    SSL_CTX *servcontext = NULL;
    if (arguments->downstream->certificate_path && arguments->downstream->key_path) {

        servcontext = handle_context(arguments->downstream->certificate_path, arguments->downstream->key_path);        

    }

    while (true) {

        struct sockaddr_in *clientaddr_in;
        int clientsock;
        if ((clientsock = accept(servsock, (struct sockaddr*) clientaddr_in, sizeof(struct sockaddr_in))) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to accept an incoming connection: '%s'.\n", strerror(errno));
            exit(EXIT_FAILURE);

        }

        // handoff to thread.

    }

    return 0;

}