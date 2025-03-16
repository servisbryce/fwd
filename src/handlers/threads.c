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

void *tls_connect() {



}

void *worker(void *worker_vargs_p) {

    worker_vargs_t *worker_vargs = (worker_vargs_t*) worker_vargs_p;
    return NULL;

}