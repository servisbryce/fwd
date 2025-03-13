#include <stddef.h>

typedef struct arguments {

    struct stream *downstream;
    struct stream *upstream;

} arguments_t;

typedef struct stream {

    uint16_t port;
    char *address;
    
} stream_t;