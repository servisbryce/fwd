#ifndef TLS_H_
#define TLS_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdbool.h>

SSL_CTX *handle_context(char *certificate_path, char *key_path, bool topology, size_t tls_cache_length, int tls_session_timeout);

#endif