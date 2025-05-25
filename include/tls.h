#ifndef TLS_H_
#define TLS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *create_client_context();

#endif