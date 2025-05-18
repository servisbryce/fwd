#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>

/* Define various protocols which are supported by the program. */
typedef enum protocol {

    GENERIC,
    HTTP

} protocol_t ;

/* Define what arguments may be passed to the program from the command-line interface. */
typedef struct arguments {

    protocol_t downstream_protocol;
    protocol_t upstream_protocol;
    uint16_t downstream_port;
    uint16_t upstream_port;
    char *downstream_address;
    char *upstream_address;

} arguments_t;

/* Define our arguments constructor. */
arguments_t construct_arguments(int argc, char *argv[]);

#endif