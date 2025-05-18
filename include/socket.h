#ifndef SOCKET_H_
#define SOCKET_H_

/* Create a socket from a socket address structure. */
int create_socket(struct sockaddr *sockaddr, int timeout);

#endif