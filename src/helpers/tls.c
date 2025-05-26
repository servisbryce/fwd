#include "../../include/tls.h"
#include <stdio.h>

/* Set a static cache identifier. */
static const char cache_id[] = "fwd";

/* Create a T.L.S. context for a client. */
SSL_CTX *create_client_context() {

    /* Create and ensure our context was created successfully. */
    SSL_CTX *tls_client_context = SSL_CTX_new(TLS_client_method());
    if (!tls_client_context) {

        fprintf(stderr, "There was an error while trying to create a T.L.S. client context.\n");
        return NULL;

    }

    /* Set our T.L.S. client settings. */
    SSL_CTX_set_verify(tls_client_context, SSL_VERIFY_PEER, NULL);
    if (!SSL_CTX_set_default_verify_paths(tls_client_context)) {

        fprintf(stderr, "There was an error while trying to enforce default certificate authority paths.\n");
        SSL_CTX_free(tls_client_context);
        return NULL;

    }

    if (!SSL_CTX_set_min_proto_version(tls_client_context, TLS1_2_VERSION)) {

        fprintf(stderr, "There was an error while trying to enforce minimum T.L.S. versions.\n");
        SSL_CTX_free(tls_client_context);
        return NULL;

    }

    return tls_client_context;

}

/* Create a T.L.S. server context. */
SSL_CTX *create_server_context(char *certificate_path, char *certificate_key_path) {

    /* Enforce parameter validation. */
    if (!certificate_path || !certificate_key_path) {

        return NULL;

    }

    /* Create our server context. */
    SSL_CTX *tls_server_context = SSL_CTX_new(TLS_server_method());
    if (!tls_server_context) {

        fprintf(stderr, "There was an error while trying to create a T.L.S. server context.\n");
        return NULL;

    }

    /* Enforce the minimum T.L.S. protocol standards. */
    if (!SSL_CTX_set_min_proto_version(tls_server_context, TLS1_2_VERSION)) {

        SSL_CTX_free(tls_server_context);
        fprintf(stderr, "There was an error while trying to enforce a minimum T.L.S. version.\n");
        return NULL;

    }

    SSL_CTX_set_security_level(tls_server_context, 0);
    long options = SSL_OP_IGNORE_UNEXPECTED_EOF | SSL_OP_NO_RENEGOTIATION | SSL_OP_CIPHER_SERVER_PREFERENCE;
    SSL_CTX_set_options(tls_server_context, options);

    /* Deploy our certificate and certificate key. */
    if (SSL_CTX_use_certificate_chain_file(tls_server_context, certificate_path) <= 0) {

        SSL_CTX_free(tls_server_context);
        fprintf(stderr, "There was an error while trying to load the certificate from the chosen certificate path.\n");
        return NULL;

    } 

    if (SSL_CTX_use_PrivateKey_file(tls_server_context, certificate_key_path, SSL_FILETYPE_PEM) <= 0) {

        SSL_CTX_free(tls_server_context);
        fprintf(stderr, "There was an error while trying to load the certificate key from the chosen certificate key path.");
        return NULL;

    } 

    /* Enforce a cache to allow for quicker client-server T.L.S. negotiations. */
    SSL_CTX_set_session_id_context(tls_server_context, (void *) cache_id, sizeof(cache_id));
    SSL_CTX_set_session_cache_mode(tls_server_context, SSL_SESS_CACHE_SERVER);
    SSL_CTX_sess_set_cache_size(tls_server_context, 32768);
    SSL_CTX_set_timeout(tls_server_context, 3600);
    
    /* Disable peer certificate verification (we're the server so there is no reason to be validating peer certificates). */
    SSL_CTX_set_verify(tls_server_context, SSL_VERIFY_NONE, NULL);

    /* Return our new context. */
    return tls_server_context;

}