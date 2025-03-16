#ifndef THREADS_H_
#define THREADS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

typedef struct tls_worker_vargs {

    SSL_CTX *context;
    size_t length;
    int sock;

} tls_worker_vargs_t;

typedef struct worker_vargs {

    size_t length;
    int sock;

} worker_vargs_t;

void *worker(void *worker_vargs_p);
void *tls_worker(void *tls_worker_vargs_p);

#endif