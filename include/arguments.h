#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stddef.h>
#include <stdint.h>

typedef struct arguments {

    struct stream *downstream;
    struct stream *upstream;
    size_t tls_cache_length;
    size_t length;
    int tls_session_timeout;

} arguments_t;

typedef struct stream {

    uint16_t port;
    size_t tls_cache_length;
    char *certificate_path;
    char *key_path;
    char *address;
    int tls_session_timeout;

} stream_t;

arguments_t *handle_arguments(int argc, char **argv);
void free_arguments(arguments_t *arguments);

#endif