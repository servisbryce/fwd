#ifndef THREADS_H_
#define THREADS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

pthread_mutex_t worker_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t worker_condition = PTHREAD_COND_INITIALIZER;

typedef struct tls_worker_vargs {

    SSL_CTX *context;
    size_t length;
    int sock;

} tls_worker_vargs_t;

typedef struct worker_vargs {

    size_t length;
    int sock;

} worker_vargs_t;

typedef struct thread {

    pthread_t id;
    void *arguments;

} thread_t;

typedef struct thread_pool {

    int available_target;
    thread_t *available;
    thread_t *busy;

} thread_pool_t;

thread_pool_t *handle_thread_pool(int available_target, SSL_CTX *context);
void free_thread_pool(thread_pool_t *thread_pool);

#endif