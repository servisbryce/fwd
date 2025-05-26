#ifndef TLS_H_
#define TLS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>

/* Create a T.L.S. context for a client. */
SSL_CTX *create_client_context();

/* Create a T.L.S. server context. */
SSL_CTX *create_server_context(char *certificate_path, char *certificate_key_path);

#endif