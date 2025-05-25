#include "../../include/cli.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Find our protocol from a string. */
protocol_t construct_protocol(char *tokenized_protocol) {

    if (strcmp(tokenized_protocol, "http") == 0) return HTTP;
    return GENERIC;

}

/* We are to process our raw arguments into structured and segmented chunks of data that we're able to use. */
arguments_t construct_arguments(int argc, char *argv[]) {

    /* Create our structure and ensure that it's memory is cleared out. */
    arguments_t arguments;
    memset(&arguments, 0, sizeof(arguments));
    arguments.maximum_connections = 4096;
    arguments.downstream_timeout = 60;
    arguments.upstream_timeout = 60;

    /* Parse the command-line arguments. */
    int flag;
    while ((flag = getopt(argc, argv, "u:d:c:k:m:f:i:")) != -1) {

        char *remaining_string = optarg;
        char *token = NULL;
        int segment = 0;
        switch (flag) {

            /* Determine the upstream address and port. */
            case 'u':

                /* Set our variables to their default values. */
                char *tokenized_upstream_port = NULL;
                remaining_string = optarg;
                token = NULL;
                segment = 0;

                /* Tokenize the string. */
                while ((token = strtok_r(remaining_string, ":", &remaining_string)) != NULL) {

                    if (segment == 0) {

                        /* Determine our protocol.*/
                        arguments.upstream_protocol = construct_protocol(token);

                        /* If it's set to generic, that means we're currently selecting our address. */
                        if (arguments.upstream_protocol == GENERIC) {

                            /* Set our address. */
                            arguments.upstream_address = token;
                            
                            /* Iterate one more so we're able to skip over the address portion as it's already selected. */
                            segment++;

                        }

                    } else if (segment == 1) {

                        /* Splice out the remaining piece of the protocol demarker. */
                        arguments.upstream_address = (char *) malloc(strlen(token) - 1);
                        strcpy(arguments.upstream_address, token + 2);

                    } else if (segment == 2) {

                        /* Set the tokenized port. */
                        tokenized_upstream_port = token;

                    }

                    /* Iterate to signify we're moving on. */
                    segment++;

                }

                /* Ensure that we were able to determine data for either string. */
                if (!tokenized_upstream_port) {

                    fprintf(stderr, "There was an error while parsing the address or port.\n");
                    exit(EXIT_FAILURE);

                }

                /* Convert the tokenized upstream port into a number rather than a string. */
                arguments.upstream_port = (uint16_t) strtol(tokenized_upstream_port, NULL, 10);
                break;
                
            /* Determine the downstream address and port. */
            case 'd':
                
                /* Set our variables to their default values. */
                char *tokenized_downstream_port = NULL;
                remaining_string = optarg;
                token = NULL;
                segment = 0;

                /* Tokenize the string. */
                while ((token = strtok_r(remaining_string, ":", &remaining_string)) != NULL) {

                    if (segment == 0) {

                        /* Determine our protocol.*/
                        arguments.downstream_protocol = construct_protocol(token);

                        /* If it's set to generic, that means we're currently selecting our address. */
                        if (arguments.downstream_protocol == GENERIC) {

                            /* Set our address. */
                            arguments.downstream_address = token;
                            
                            /* Iterate one more so we're able to skip over the address portion as it's already selected. */
                            segment++;

                        }

                    } else if (segment == 1) {

                        /* Splice out the remaining piece of the protocol demarker. */
                        arguments.downstream_address = (char *) malloc(strlen(token) - 1);
                        strcpy(arguments.downstream_address, token + 2);

                    } else if (segment == 2) {

                        /* Set the tokenized port. */
                        tokenized_downstream_port = token;

                    }

                    /* Iterate to signify we're moving on. */
                    segment++;

                }

                /* Ensure that we were able to determine data for either string. */
                if (!tokenized_downstream_port) {

                    fprintf(stderr, "There was an error while parsing the address or port.\n");
                    exit(EXIT_FAILURE);

                }

                /* Convert the tokenized upstream port into a number rather than a string. */
                arguments.downstream_port = (uint16_t) strtol(tokenized_downstream_port, NULL, 10);
                break;

            case 'c':
                arguments.upstream_certificate = optarg;
                break;

            case 'k':
                arguments.upstream_certificate_key = optarg;
                break;

            case 'm':
                arguments.maximum_connections = (uint16_t) strtol(optarg, NULL, 10);
                break;

            case 'f':
                arguments.downstream_timeout = (int) strtol(optarg, NULL, 10);
                break;
            
            case 'i':
                arguments.upstream_timeout = (int) strtol(optarg, NULL, 10);
                break;

        }

    }

    /* Check if upstream and downstream values were set. */
    if (!arguments.upstream_address || !arguments.downstream_address) {

        fprintf(stderr, "You must specify both a downstream and upstream bind address.\n");
        exit(EXIT_FAILURE);

    }

    /* Lint our user-inputted data to ensure that there's no configuration errors. */
    if (!arguments.upstream_certificate || !arguments.upstream_certificate_key) {

        fprintf(stderr, "If you've specified that you'll be using an encrypted protocol upstream, then you'll need to specify a certificate and a certificate key!\n");
        exit(EXIT_FAILURE);

    }

    /* Check that our various integer values actually make sense. */
    if (arguments.maximum_connections == 0 || arguments.upstream_port == 0 || arguments.downstream_port == 0) {

        fprintf(stderr, "You cannot set the maximum connections, upstream port, or downstream port to a value that's below zero.\n");
        exit(EXIT_FAILURE);

    }

    return arguments;

}