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

void tls_receive(int servsock, SSL_CTX *servcontext, thread_pool_t *thread_pool) {

    while (true) {

        struct sockaddr_in *clientaddr_in = NULL;
        unsigned int clientaddr_in_length = sizeof(struct sockaddr_in);
        int clientsock;
        if ((clientsock = accept(servsock, (struct sockaddr*) clientaddr_in, &clientaddr_in_length)) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to accept an incoming connection: '%s'.\n", strerror(errno));
            exit(EXIT_FAILURE);

        }

        exit(EXIT_FAILURE);

    }

}

void receive(int servsock, thread_pool_t *thread_pool) {

    while (true) {

        struct sockaddr_in *clientaddr_in = NULL;
        unsigned int clientaddr_in_length = sizeof(struct sockaddr_in);
        int clientsock;
        if ((clientsock = accept(servsock, (struct sockaddr*) clientaddr_in, &clientaddr_in_length)) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to accept an incoming connection: '%s'.\n", strerror(errno));
            exit(EXIT_FAILURE);

        }

        pthread_t id = thread_pool->available[0].id;
        pthread_mutex_lock(&worker_mutex);
        pthread_cond_signal(&worker_condition);
        pthread_mutex_unlock(&worker_mutex);

    }

}


int main(int argc, char **argv) {

    arguments_t *arguments = NULL;
    arguments = handle_arguments(argc, argv);
    struct sockaddr_in *servaddr_in = NULL;
    servaddr_in = handle_sockaddr_in(arguments->downstream->address, arguments->downstream->port);
    int servsock = handle_socket(servaddr_in);
    SSL_CTX *servcontext = NULL;
    if (arguments->downstream->certificate_path && arguments->downstream->key_path) {

        servcontext = handle_context(arguments->downstream->certificate_path, arguments->downstream->key_path);        

    }

    thread_pool_t *thread_pool = NULL;
    thread_pool = handle_thread_pool(64, servcontext);
    if (!servcontext) {

        receive(servsock, thread_pool);

    } else {

        tls_receive(servsock, servcontext, thread_pool);

    }

    close(servsock);
    free(servaddr_in);
    free_arguments(arguments);
    if (servcontext) {

        SSL_CTX_free(servcontext);

    }

    return 0;

}