#ifndef THREADS_H_
#define THREADS_H_

#include "arguments.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

typedef struct tls_worker_vargs {

    stream_t *upstream;
    SSL_CTX *servcontext;
    SSL_CTX *clientcontext;
    size_t buffer_length;
    int sock;

} tls_worker_vargs_t;

typedef struct worker_vargs {

    stream_t *upstream;
    size_t buffer_length;
    int sock;

} worker_vargs_t;

void *worker(void *worker_vargs_p);
void *tls_worker(void *tls_worker_vargs_p);

#endif