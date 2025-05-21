/*

    Forward

*/

#include "../include/sockaddr.h"
#include "../include/generic.h"
#include "../include/socket.h"
#include "../include/https.h"
#include "../include/http.h"
#include "../include/cli.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    /* Parse command-line arguments into an actual structure. */
    arguments_t arguments = construct_arguments(argc, argv);

    /* Construct our downstream socket address. */
    struct sockaddr *downstream_sockaddr = construct_sockaddr(arguments.downstream_address, arguments.downstream_port);

    /* Construct our upstream socket address. */
    struct sockaddr *upstream_sockaddr = construct_sockaddr(arguments.upstream_address, arguments.upstream_port);
    
    /* Construct our upstream-facing listener socket. */
    int sockfd = create_socket(upstream_sockaddr, -1);

    /* This is temporary to test the unprotected generic interface! */
    unprotected_generic_interface(downstream_sockaddr, upstream_sockaddr, sockfd, arguments.upstream_timeout);

    /* Return success if we reach this point. */
    return 0;

}