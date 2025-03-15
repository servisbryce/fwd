#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stddef.h>
#include <stdint.h>

typedef struct arguments {

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

arguments_t *handle_arguments(int argc, char **argv);

#endif