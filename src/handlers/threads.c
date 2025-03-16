#include "../include/threads.h"
#include "../include/sockets.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void *tls_worker(void *tls_worker_vargs_p) {

    tls_worker_vargs_t *tls_worker_vargs = (tls_worker_vargs_t*) tls_worker_vargs_p;
    return NULL;

}

void *tls_upstream_connect(char *address, uint16_t port, SSL_CTX *clientcontext, void *request_buffer, size_t request_buffer_length, size_t buffer_length, size_t *response_buffer_length) {

    struct sockaddr_in *upstream_addr = handle_sockaddr_in(address, port);
    int upstream_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(upstream_sock, (struct sockaddr*) upstream_addr, sizeof(struct sockaddr_in)) < 0) {

        fprintf(stderr, "There was an error while trying to connect to the upstream server: '%s'.\n", strerror(errno));
        return NULL;

    }

    SSL *tls = SSL_new(clientcontext);
    SSL_set_fd(tls, upstream_sock);
    SSL_set_connect_state(tls);
    if (SSL_connect(tls) <= 0) {

        fprintf(stderr, "There was an error while trying to establish security with the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        SSL_free(tls);
        free(upstream_addr);
        return NULL;

    }

    if (SSL_write(tls, request_buffer, request_buffer_length) <= 0) {

        fprintf(stderr, "There was an error while trying to send data to the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        SSL_free(tls);
        free(upstream_addr);
        return NULL;

    }

    void *response_buffer = malloc(buffer_length);
    size_t response_length;
    if (SSL_read_ex(tls, response_buffer, buffer_length, &response_length) <= 0) {

        fprintf(stderr, "There was an error while trying to receive data from the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        SSL_free(tls);
        free(response_buffer);
        free(upstream_addr);
        return NULL;

    }

    response_buffer_length = &response_length;
    realloc(response_buffer, response_length);
    close(upstream_sock);
    SSL_free(tls);
    free(upstream_addr);
    return response_buffer;

}

void *worker(void *worker_vargs_p) {

    worker_vargs_t *worker_vargs = (worker_vargs_t*) worker_vargs_p;
    return NULL;

}

void *upstream_connect(char *address, uint16_t port, void *request_buffer, size_t request_buffer_length, size_t buffer_length, size_t *response_buffer_length) {

    struct sockaddr_in *upstream_addr = handle_sockaddr_in(address, port);
    int upstream_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(upstream_sock, (struct sockaddr*) upstream_addr, sizeof(struct sockaddr_in)) < 0) {

        fprintf(stderr, "There was an error while trying to connect to the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        return NULL;

    }

    if (write(upstream_sock, request_buffer, request_buffer_length) < 0) {

        fprintf(stderr, "There was an error while trying to send data to the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        return NULL;

    }

    void *response_buffer = malloc(buffer_length);
    size_t response_length;
    if ((response_length = read(upstream_sock, response_buffer, buffer_length)) < 0) {

        fprintf(stderr, "There was an error while trying to receive data from the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        free(response_buffer);
        return NULL;

    }

    response_buffer_length = &response_length;
    realloc(response_buffer, response_length);
    close(upstream_sock);
    free(upstream_addr);
    return response_buffer;

}