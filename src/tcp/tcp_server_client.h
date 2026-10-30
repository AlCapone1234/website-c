#ifndef TCP_SERVER_CLIENT_H
#define TCP_SERVER_CLIENT_H

#include "tcp_error.h"

#include <sys/socket.h> 
#include <netinet/in.h> 

#ifndef TCP_SERVER_CLIENT_MAX_BUFFER
    #define TCP_SERVER_CLIENT_MAX_BUFFER 4096
#endif 

typedef struct
{
    struct sockaddr_in addr;
    socklen_t addr_length;
    int fd;
    char outgoing_buffer[TCP_SERVER_CLIENT_MAX_BUFFER];
    size_t total_outgoing_buffer;
} TCP_Server_Client;

TCP_Error TCP_Server_Client_Initialize(TCP_Server_Client* client);

#endif