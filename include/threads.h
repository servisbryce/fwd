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

typedef struct thread_pool {

    int available_target;
    pthread_t *available;
    pthread_t *busy;

} thread_pool_t;

void free_thread_pool(thread_pool_t *thread_pool);