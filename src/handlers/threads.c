#include "../include/threads.h"
#include "../include/sockets.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void *tls_worker(void *tls_worker_vargs_p) {

    tls_worker_vargs_t *tls_worker_vargs = (tls_worker_vargs_t*) tls_worker_vargs_p;
    SSL *tunnel = SSL_new(tls_worker_vargs->context);
    SSL_set_fd(tunnel, tls_worker_vargs->sock);
    if (SSL_accept(tunnel) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to accept an incoming T.L.S. connection: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);

    }

    void *request_buffer = malloc(tls_worker_vargs->buffer_length);
    size_t request_length;
    SSL_read_ex(tunnel, request_buffer, tls_worker_vargs->buffer_length, &request_length);
    realloc(request_buffer, request_length);
    printf("%s\n", request_buffer);
    printf("%ld\n", request_length);
    SSL_shutdown(tunnel);
    SSL_free(tunnel);
    close(tls_worker_vargs->sock);
    return NULL;

}

void *worker(void *worker_vargs_p) {

    worker_vargs_t *worker_vargs = (worker_vargs_t*) worker_vargs_p;
    void *request_buffer = malloc(worker_vargs->buffer_length);
    size_t request_length = read(worker_vargs->sock, request_buffer, worker_vargs->buffer_length);
    realloc(request_buffer, request_length);
    printf("%s\n", request_buffer);
    int clientsock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in *clientaddr = NULL;
    unsigned int clientaddr_in_length = sizeof(struct sockaddr_in);
    clientaddr = handle_sockaddr_in(worker_vargs->upstream->address, worker_vargs->upstream->port);
    if (connect(clientsock, (struct sockaddr*) clientaddr, clientaddr_in_length) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to connect to the upstream server: '%s'\n", strerror(errno));

    }

    write(clientsock, request_buffer, request_length);
    void *response_buffer = malloc(worker_vargs->buffer_length);
    size_t response_length = read(clientsock, response_buffer, worker_vargs->buffer_length);
    realloc(response_buffer, response_length);
    write(worker_vargs->sock, response_buffer, response_length);
    close(clientsock);
    close(worker_vargs->sock);
    return NULL;

}