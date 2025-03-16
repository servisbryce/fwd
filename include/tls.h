#ifndef TLS_H_
#define TLS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *handle_context(char *certificate_path, char *key_path);

#endif