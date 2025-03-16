#include "../include/threads.h"
#include <signal.h>
#include <errno.h>

void *tls_worker(void *tls_worker_vargs_p) {



}

void *worker(void *worker_vargs_p) {

    pthread_mutex_lock(&worker_mutex);
    pthread_cond_wait(&worker_condition, &worker_mutex);
    printf("we have solid copy.");
    pthread_mutex_unlock(&worker_mutex);
    return NULL;

}

thread_pool_t *handle_thread_pool(int available_target, SSL_CTX *context) {

    size_t array_size = available_target * sizeof(thread_t);
    thread_pool_t *thread_pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
    thread_pool->available = (thread_t*) malloc(array_size);
    thread_pool->busy = (thread_t*) malloc(array_size); 
    thread_pool->available_target = available_target;
    if (context) {

        for (int i = 0; i < available_target; i++) {

            thread_t thread;
            thread.arguments = malloc(sizeof(tls_worker_vargs_t));
            pthread_create(&thread.id, NULL, tls_worker, thread.arguments);
            pthread_detach(thread.id);
            thread_pool->available[i] = thread;

        }

    } else {

        for (int i = 0; i < available_target; i++) {

            thread_t thread;
            thread.arguments = malloc(sizeof(worker_vargs_t));
            pthread_create(&thread.id, NULL, worker, thread.arguments);
            pthread_detach(thread.id);
            thread_pool->available[i] = thread;

        }

    }

    return thread_pool;

}

void free_thread_pool(thread_pool_t *thread_pool) {

    free(thread_pool->available);
    free(thread_pool->busy);
    free(thread_pool);

}