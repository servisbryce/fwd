/*

    Forward

*/

#include "../include/https.h"
#include "../include/http.h"
#include "../include/cli.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    arguments_t arg = construct_arguments(argc, argv);
    printf("%s\n", arg.upstream_address);

}