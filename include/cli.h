#ifndef CLI_H_
#define CLI_H_

/* Define various protocols which are supported by the program. */
typedef enum protocol {

    GENERIC,
    HTTPS,
    HTTP

} protocol_t ;

/* Define what arguments may be passed to the program from the command-line interface. */
typedef struct arguments {

    protocol_t protocol;
    uint16_t port;
    char *address;

} arguments_t;

#endif