#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {

    HTTP,
    GENERIC

} protocols_t;

typedef struct arguments {

    protocols_t protocol;
    struct stream *downstream;
    struct stream *upstream;
    size_t length;

} arguments_t;

typedef struct stream {

    uint16_t port;
    char *certificate_path;
    char *key_path;
    char *address;

} stream_t;

void handle_arguments(int argc, char **argv, arguments_t *arguments);

#endif