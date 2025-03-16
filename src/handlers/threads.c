#include "../include/threads.h"
#include <signal.h>
#include <errno.h>

void *tls_worker(void *tls_worker_vargs_p) {



}

void *worker(void *worker_vargs_p) {

    sigset_t *set = NULL;
    int *sig = NULL;
    if (sigwait(set, sig) == 0) {

        printf("Signal received, sir yes sir.");

    } else {

        fprintf(stderr, "There was an unexpected error while trying to receive a signal: '%s'.\n", strerror(errno));

    }

    return NULL;

}

thread_pool_t *handle_thread_pool(int available_target) {

    size_t array_size = available_target * sizeof(pthread_t);
    thread_pool_t *thread_pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
    thread_pool->available = (pthread_t*) malloc(array_size);
    thread_pool->busy = (pthread_t*) malloc(array_size); 
    thread_pool->available_target = available_target;

}

void free_thread_pool(thread_pool_t *thread_pool) {

    free(thread_pool->available);
    free(thread_pool->busy);
    free(thread_pool);

}