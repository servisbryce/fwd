#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>
#include <stdbool.h>

SSL_CTX *handle_context(char *certificate_path, char *key_path, bool topology, size_t tls_cache_length, int tls_session_timeout) {

    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms();
    SSL_METHOD *method = NULL;
    if (topology) {

        method = TLS_server_method();

    } else {

        method = TLS_client_method();

    }

    SSL_CTX *context = SSL_CTX_new(method);
    if (!context) {

        fprintf(stderr, "There was an error while constructing a Transport Layer Security context: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);

    }

    if (SSL_CTX_use_certificate_file(context, certificate_path, SSL_FILETYPE_PEM) <= 0) {

        fprintf(stderr, "There was an error while assigning a certificate to a Transport Layer Security context: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);

    }

    if (SSL_CTX_use_PrivateKey_file(context, key_path, SSL_FILETYPE_PEM) <= 0) {

        fprintf(stderr, "There was an error while assigning a private key to a Transport Layer Security context: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);

    }

    /* Enforce minimum versions and options for Transport Layer Security. */
    if (!SSL_CTX_set_min_proto_version(context, TLS1_2_VERSION)) {

        fprintf(stderr, "There was an error while assigning a minimum version to a Transport Layer Security context: '%s'.\n", strerror(errno));
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);

    }

    long tls_options = SSL_OP_IGNORE_UNEXPECTED_EOF | SSL_OP_NO_RENEGOTIATION | SSL_OP_CIPHER_SERVER_PREFERENCE;
    SSL_CTX_set_options(context, tls_options);

    /* Provide Transport Layer Security session caching to reduce latency. */
    SSL_CTX_sess_set_cache_size(context, tls_cache_length);
    SSL_CTX_set_timeout(context, tls_session_timeout);

    return context;

}