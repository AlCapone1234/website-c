#ifndef TCP_SERVER_CLIENT_H
#define TCP_SERVER_CLIENT_H

#include <sys/socket.h> 
#include <netinet/in.h> 

typedef struct
{
    struct sockaddr_in addr;
    socklen_t addr_length;
    int fd;
} TCP_Server_Client;

void TCP_Server_Client_Initialize(TCP_Server_Client* client);

#endif