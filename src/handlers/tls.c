#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>

void handle_context(SSL_CTX *context, char *certificate_path, char *key_path) {

    SSL_METHOD *method = TLS_server_method();
    context = SSL_CTX_new(method);
    if (!context) {

        fprintf(stderr, "There was an error while constructing a Transport Layer Security context: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (SSL_CTX_use_certificate_file(context, certificate_path, SSL_FILETYPE_PEM) < 0) {

        fprintf(stderr, "There was an error while assigning a certificate to a Transport Layer Security context: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

    if (SSL_CTX_use_PrivateKey_file(context, key_path, SSL_FILETYPE_PEM) < 0) {

        fprintf(stderr, "There was an error while assigning a private key to a Transport Layer Security context: '%s'.", strerror(errno));
        exit(EXIT_FAILURE);

    }

}