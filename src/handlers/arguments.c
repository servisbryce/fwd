#include "../include/arguments.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

protocols_t match_protocol(char *segment) {

    if (strcmp(segment, "http") == 0) return HTTP;
    else return GENERIC;

}

void handle_arguments(int argc, char **argv, arguments_t *arguments) {

    /* Position arguments are required.*/
    if (!argv[1]) {

        fprintf(stderr, "The upstream address and port must be defined.\n");
        exit(EXIT_FAILURE);

    }

    /* Prevent uninitalized pointer errors in the downstream structure. */
    arguments->downstream = (stream_t*) malloc(sizeof(stream_t));
    arguments->downstream->address = NULL;
    arguments->downstream->certificate_path = NULL;
    arguments->downstream->key_path = NULL;

    /* Prevent uninitalized pointer errors in the upstream structure while also parsing the required positional arguments. */
    arguments->upstream = (stream_t*) malloc(sizeof(stream_t));
    arguments->upstream->address = strtok(argv[1], ":");
    arguments->upstream->certificate_path = NULL;
    arguments->upstream->key_path = NULL;

    /* Parse the upstream port. */
    char *upstream_port = strtok(NULL, ":");
    if (!upstream_port) {

        fprintf(stderr, "The upstream port must be defined.\n");
        exit(EXIT_FAILURE);

    }

    arguments->upstream->port = (uint16_t) strtol(upstream_port, NULL, 10);

    /* Parse optional arguments. */
    int option;
    while ((option = getopt(argc, argv, "d:f:p:")) != -1) {

        switch (option) {

            /* Downstream address argument.*/
            case 'd':
                arguments->downstream->address = optarg;
                break;

            /* Downstream port argument. */
            case 'f':
                arguments->downstream->port = strtol(optarg, &optarg, 10);
                break;

            /* Protocol argument. */
            case 'p':
                arguments->protocol = match_protocol(optarg);
                break;

            /* Maximum Transmission Control Protocol buffer length. */
            case 'l':
                arguments->length = (size_t) strtoll(optarg, &optarg, 10);
                break;

            /* Path for downstream certificate. */
            case 'g':
                arguments->downstream->certificate_path = optarg;
                break;

            /* Path for downstream key. */
            case 'h':
                arguments->downstream->key_path = optarg;
                break;
            
            /* Path for upstream certificate. */
            case 'u':
                arguments->upstream->certificate_path = optarg;
                break;

            /* Path for upstream key. */
            case 'i':
                arguments->upstream->key_path = optarg;
                break;

            /* Unknown argument. */
            case '?':
                exit(EXIT_FAILURE);
                break;

            default:
                printf("Usage:\n");
                printf("fwd [upstream address:upstream port] [options]");
                printf("\n");
                printf("DOWNSTREAM OPTIONS\n");
                printf(" -d             fwd bind address\n");
                printf(" -f             fwd bind port\n");
                printf(" -p             protocol\n");
                printf("PROTOCOLS\n");
                printf(" unspecified    defaults to raw tcp\n");
                printf(" http           http protocol (or https)\n");
                break;


        }

    }

    /* If optional arguments are undefined, assign sane defaults. */
    if (!arguments->downstream->address) arguments->downstream->address = "127.0.0.1";
    if (!arguments->downstream->port) arguments->downstream->port = arguments->upstream->port;
    if (!arguments->protocol) arguments->protocol = GENERIC;
    if (!arguments->length) arguments->length = 65536;

}