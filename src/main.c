#include "../include/arguments.h"
#include "../include/sockets.h"
#include "../include/threads.h"
#include "../include/tls.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void tls_receive(int servsock, SSL_CTX *servcontext, SSL_CTX *clientcontext, arguments_t *arguments) {

    while (true) {

        struct sockaddr_in *clientaddr_in = NULL;
        unsigned int clientaddr_in_length = sizeof(struct sockaddr_in);
        int clientsock;
        if ((clientsock = accept(servsock, (struct sockaddr*) clientaddr_in, &clientaddr_in_length)) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to accept an incoming connection: '%s'.\n", strerror(errno));

        }

        pthread_t thread_id;
        tls_worker_vargs_t *tls_worker_vargs = (tls_worker_vargs_t*) malloc(sizeof(tls_worker_vargs_t));
        tls_worker_vargs->upstream = arguments->upstream;
        tls_worker_vargs->servcontext = servcontext;
        tls_worker_vargs->clientcontext = clientcontext;
        tls_worker_vargs->buffer_length = arguments->length;
        tls_worker_vargs->sock = clientsock;
        pthread_create(&thread_id, NULL, tls_worker, (void*) tls_worker_vargs);
        pthread_detach(thread_id);

    }

}

void receive(int servsock, arguments_t *arguments, SSL_CTX *clientcontext) {

    while (true) {

        struct sockaddr_in *clientaddr_in = NULL;
        unsigned int clientaddr_in_length = sizeof(struct sockaddr_in);
        int clientsock;
        if ((clientsock = accept(servsock, (struct sockaddr*) clientaddr_in, &clientaddr_in_length)) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to accept an incoming connection: '%s'.\n", strerror(errno));

        }

        pthread_t thread_id;
        worker_vargs_t *worker_vargs = (worker_vargs_t*) malloc(sizeof(worker_vargs_t));
        worker_vargs->upstream = arguments->upstream;
        worker_vargs->clientcontext = clientcontext;
        worker_vargs->buffer_length = arguments->length;
        worker_vargs->sock = clientsock;
        pthread_create(&thread_id, NULL, worker, (void*) worker_vargs);
        pthread_detach(thread_id);

    }

}

int main(int argc, char **argv) {

    arguments_t *arguments = NULL;
    arguments = handle_arguments(argc, argv);
    struct sockaddr_in *servaddr_in = NULL;
    servaddr_in = handle_sockaddr_in(arguments->downstream->address, arguments->downstream->port);
    int servsock = handle_socket(servaddr_in);
    SSL_CTX *clientcontext = NULL;
    SSL_CTX *servcontext = NULL;
    if (arguments->downstream->certificate_path && arguments->downstream->key_path) {

        servcontext = handle_context(arguments->downstream->certificate_path, arguments->downstream->key_path, true);        

    }

    if (arguments->upstream->certificate_path && arguments->upstream->key_path) {

        clientcontext = handle_context(arguments->upstream->certificate_path, arguments->upstream->key_path, false);

    }

    if (!servcontext) {

        receive(servsock, arguments, clientcontext);

    } else {

        tls_receive(servsock, servcontext, clientcontext, arguments);

    }

    close(servsock);
    free(servaddr_in);
    free_arguments(arguments);
    if (servcontext) {

        SSL_CTX_free(servcontext);

    }

    if (clientcontext) {

        SSL_CTX_free(clientcontext);

    }

    return 0;

}