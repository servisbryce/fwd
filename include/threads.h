#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct tls_worker_vargs {

    SSL_CTX *context;
    int sock;

} tls_worker_vargs_t;

typedef struct worker_vargs {

    int sock;

} worker_vargs_t;