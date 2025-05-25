#include "../../include/tls.h"
#include <stdio.h>

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

