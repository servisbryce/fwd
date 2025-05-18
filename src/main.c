/*

    Forward

*/

#include "../include/sockaddr.h"
#include "../include/https.h"
#include "../include/http.h"
#include "../include/cli.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    /* Parse command-line arguments into an actual structure. */
    arguments_t arguments = construct_arguments(argc, argv);

    /* Construct our downstream socket address. */
    struct sockaddr *downstream_sockaddr = construct_sockaddr(arguments.downstream_address, arguments.downstream_port);

    /* Construct our upstream socket address. */
    struct sockaddr *upstream_sockaddr = construct_sockaddr(arguments.upstream_address, arguments.upstream_port);
    

}