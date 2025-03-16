#include "../include/threads.h"
#include "../include/sockets.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void *tls_downstream_serve(int sock, SSL_CTX *servcontext, size_t buffer_length, size_t *request_buffer_length, SSL **tls_handshake) {

    SSL *tls = SSL_new(servcontext);
    SSL_set_fd(tls, sock);
    if (SSL_accept(tls) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to establish security with an incoming connection: '%s'\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        SSL_free(tls);
        return NULL;

    }

    void *request_buffer = malloc(buffer_length);
    size_t request_length;
    if (SSL_read_ex(tls, request_buffer, buffer_length, &request_length) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to read data from an incoming connection: '%s'\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        SSL_free(tls);
        free(request_buffer);
        return NULL;

    }

    realloc(request_buffer, request_length);
    *request_buffer_length = request_length;
    *tls_handshake = tls;
    return request_buffer;

}

void *tls_upstream_connect(char *address, uint16_t port, SSL_CTX *clientcontext, void *request_buffer, size_t request_buffer_length, size_t buffer_length, size_t *response_buffer_length) {

    struct sockaddr_in *upstream_addr = handle_sockaddr_in(address, port);
    int upstream_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(upstream_sock, (struct sockaddr*) upstream_addr, sizeof(struct sockaddr_in)) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to connect to the upstream server: '%s'.\n", strerror(errno));
        return NULL;

    }

    SSL *tls = SSL_new(clientcontext);
    SSL_set_fd(tls, upstream_sock);
    SSL_set_tlsext_host_name(tls, address); 
    if (SSL_connect(tls) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to establish security with the upstream server: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        close(upstream_sock);
        SSL_shutdown(tls);
        free(upstream_addr);
        return NULL;

    }

    if (SSL_write(tls, request_buffer, request_buffer_length) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to send data to the upstream server: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        close(upstream_sock);
        SSL_shutdown(tls);
        free(upstream_addr);
        return NULL;

    }

    void *response_buffer = malloc(buffer_length);
    size_t response_length;
    if (SSL_read_ex(tls, response_buffer, buffer_length, &response_length) <= 0) {

        fprintf(stderr, "There was an unexpected error while trying to receive data from the upstream server: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        close(upstream_sock);
        SSL_shutdown(tls);
        free(response_buffer);
        free(upstream_addr);
        return NULL;

    }

    *response_buffer_length = response_length;
    realloc(response_buffer, response_length);
    close(upstream_sock);
    SSL_shutdown(tls);
    free(upstream_addr);
    return response_buffer;

}

void *downstream_serve(int sock, size_t buffer_length, size_t *request_buffer_length) {

    void *request_buffer = malloc(buffer_length);
    size_t request_length;
    if ((request_length = read(sock, request_buffer, buffer_length)) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to receive data from an incoming connection: '%s'.\n", strerror(errno));
        free(request_buffer);
        return NULL;

    }

    *request_buffer_length = request_length;
    realloc(request_buffer, request_length);
    return request_buffer;

} 

void *upstream_connect(char *address, uint16_t port, void *request_buffer, size_t request_buffer_length, size_t buffer_length, size_t *response_buffer_length) {

    struct sockaddr_in *upstream_addr = handle_sockaddr_in(address, port);
    int upstream_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(upstream_sock, (struct sockaddr*) upstream_addr, sizeof(struct sockaddr_in)) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to connect to the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        return NULL;

    }

    if (write(upstream_sock, request_buffer, request_buffer_length) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to send data to the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        return NULL;

    }

    void *response_buffer = malloc(buffer_length);
    size_t response_length;
    if ((response_length = read(upstream_sock, response_buffer, buffer_length)) < 0) {

        fprintf(stderr, "There was an unexpected error while trying to receive data from the upstream server: '%s'.\n", strerror(errno));
        close(upstream_sock);
        free(upstream_addr);
        free(response_buffer);
        return NULL;

    }

    *response_buffer_length = response_length;
    realloc(response_buffer, response_length);
    close(upstream_sock);
    free(upstream_addr);
    return response_buffer;

}

void *tls_worker(void *tls_worker_vargs_p) {

    tls_worker_vargs_t *tls_worker_vargs = (tls_worker_vargs_t*) tls_worker_vargs_p;
    size_t request_length;
    SSL *tls = NULL;
    void *request_buffer = tls_downstream_serve(tls_worker_vargs->sock, tls_worker_vargs->servcontext, tls_worker_vargs->buffer_length, &request_length, &tls);
    if (!request_buffer) {

        return NULL;

    }

    printf("%s\n", request_buffer);
    size_t response_length;
    void *response_buffer = NULL;
    if (tls_worker_vargs->clientcontext) {

        response_buffer = tls_upstream_connect(tls_worker_vargs->upstream->address, tls_worker_vargs->upstream->port, tls_worker_vargs->clientcontext, request_buffer, request_length, tls_worker_vargs->buffer_length, &response_length);

    } else {

        response_buffer = upstream_connect(tls_worker_vargs->upstream->address, tls_worker_vargs->upstream->port, request_buffer, request_length, tls_worker_vargs->buffer_length, &response_length);

    }

    printf("%s\n",response_buffer);
    if (!response_buffer) {

        // return a TCP error.

    } else {

        if (SSL_write(tls, response_buffer, response_length) <= 0) {

            fprintf(stderr, "There was an unexpected error while trying to send data to an incoming connection: '%s'.\n", strerror(errno));
            ERR_print_errors_fp(stderr);
            SSL_shutdown(tls);
            return NULL;

        }

    }

    SSL_shutdown(tls);
    SSL_free(tls);
    close(tls_worker_vargs->sock);
    return NULL;

}

void *worker(void *worker_vargs_p) {

    worker_vargs_t *worker_vargs = (worker_vargs_t*) worker_vargs_p;
    size_t request_length;
    void *request_buffer = downstream_serve(worker_vargs->sock, worker_vargs->buffer_length, &request_length);
    if (!request_buffer) {

        return NULL;

    }

    size_t response_length;
    void *response_buffer = NULL;
    if (worker_vargs->clientcontext) {

        response_buffer = tls_upstream_connect(worker_vargs->upstream->address, worker_vargs->upstream->port, worker_vargs->clientcontext, request_buffer, request_length, worker_vargs->buffer_length, &response_length);

    } else {

        response_buffer = upstream_connect(worker_vargs->upstream->address, worker_vargs->upstream->port, request_buffer, request_length, worker_vargs->buffer_length, &response_length);

    }

    if (!response_buffer) {

        // report TCP error to client.

    } else {

        if (write(worker_vargs->sock, response_buffer, response_length) < 0) {

            fprintf(stderr, "There was an unexpected error while trying to send data to an incoming connection: '%s'.\n", strerror(errno));
            return NULL;

        }

    }

    close(worker_vargs->sock);
    return NULL;

}