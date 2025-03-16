#include "../include/threads.h"
#include <signal.h>
#include <errno.h>

void *tls_worker(void *tls_worker_vargs_p) {



}

void *worker(void *worker_vargs_p) {

    worker_vargs_t *arguments = (worker_vargs_t*) worker_vargs_p;
    printf("AHHH!\n");
    return NULL;

}