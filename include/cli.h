#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>

/* Define various protocols which are supported by the program. */
typedef enum protocol {

    /* Regular T.C.P. without any protocol-specific enhancements. */
    GENERIC,

    /* Regular H.T.T.P. protocol-specific enhancements. */
    HTTP

} protocol_t;

/* Define what arguments may be passed to the program from the command-line interface. */
typedef struct arguments {

    /* Protocols to indicate if translation is needed from protocol-to-protocol. */
    protocol_t downstream_protocol;
    protocol_t upstream_protocol;

    /* Upstream and downstream network ports. */
    uint16_t downstream_port;
    uint16_t upstream_port;

    /* Maximum amount of connections at once. */
    uint16_t maximum_connections;

    /* The I.P. addresses of the upstream (bind) and downstream (server being proxied). */
    char *downstream_address;
    char *upstream_address;

    /* If set, these will contain the file paths for the upstream certificate and that certificates keys. */
    char *upstream_certificate;
    char *upstream_certificate_key;

    /*  These will contain the timeout for both the downstream and upstream connections regarding read and write inactivity timeouts. */
    int upstream_timeout;
    int downstream_timeout;

    /* Specify if that the downstream is protected or not. */
    bool protected_downstream;

} arguments_t;

/* Define our arguments constructor. */
arguments_t construct_arguments(int argc, char *argv[]);

#endif