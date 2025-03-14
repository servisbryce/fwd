#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {

    HTTP,
    GENERIC

} protocols_t;

typedef struct arguments {

    struct stream *downstream;
    struct stream *upstream;
    protocols_t protocol;

} arguments_t;

typedef struct stream {

    uint16_t port;
    char *address;
    
} stream_t;

void handle_arguments(int argc, char **argv, arguments_t *arguments);

#endif