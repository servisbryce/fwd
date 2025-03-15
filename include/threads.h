#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct tls_worker_vargs {

    SSL_CTX *context;
    size_t length;
    int sock;

} tls_worker_vargs_t;

typedef struct worker_vargs {

    size_t length;
    int sock;

} worker_vargs_t;