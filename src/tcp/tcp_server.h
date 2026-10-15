#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcp_server_client.h"

#include <stdint.h>
#include <stddef.h>

#ifndef TCP_SERVER_CLIENTS_CAPACITY_START
    #define TCP_SERVER_CLIENTS_CAPACITY_START 4
#endif

typedef enum
{
    TCP_Error_Success = 0,
    TCP_Error_Socket,
    TCP_Error_Bind,
    TCP_Error_Fcntl,
    TCP_Error_Listen,
    TCP_Error_Client_Is_NULL,
    TCP_Error_Server_Is_NULL,
    TCP_Error_Malloc,
    TCP_Error_Realloc,
    TCP_Error_Clients_Capacity_Is_Less_Than_One,
} TCP_Error;

typedef struct 
{
    TCP_Server_Client* clients;
    size_t clients_length;
    size_t clients_capacity;
} TCP_Server;

TCP_Error TCP_Server_Initialize(TCP_Server* server, uint16_t port);
TCP_Error TCP_Server_Add_Client(TCP_Server* server, TCP_Server_Client* client);

#endif