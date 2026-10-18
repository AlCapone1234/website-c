#ifndef TCP_SERVER_CLIENT_H
#define TCP_SERVER_CLIENT_H

#include "tcp_error.h"

#include <sys/socket.h> 
#include <netinet/in.h> 

typedef struct
{
    struct sockaddr_in addr;
    socklen_t addr_length;
    int fd;
} TCP_Server_Client;

TCP_Error TCP_Server_Client_Initialize(TCP_Server_Client* client);

#endif