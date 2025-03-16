#ifndef SOCKETS_H_
#define SOCKETS_H_

struct sockaddr_in *handle_sockaddr_in(char *address, uint16_t port);
int handle_socket(struct sockaddr_in *sockaddr);

#endif