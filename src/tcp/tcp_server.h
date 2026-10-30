#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcp_server_client.h"
#include "tcp_error.h"

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

#ifndef TCP_SERVER_CLIENTS_CAPACITY_START
    #define TCP_SERVER_CLIENTS_CAPACITY_START 4
#endif

typedef struct TCP_Server TCP_Server;
typedef void (*TCP_Server_On_Initialized)(TCP_Server*);
typedef void (*TCP_Server_On_Shutdown)(TCP_Server*);

typedef struct
{
    TCP_Server_On_Initialized on_initialized;
    TCP_Server_On_Shutdown on_shutdown;
} TCP_Server_Callbacks;

struct TCP_Server
{
    TCP_Server_Client* clients;
    size_t clients_length;
    size_t clients_capacity;

    TCP_Server_Callbacks* callbacks;    
};

TCP_Error TCP_Server_Initialize(TCP_Server* server, uint16_t port, TCP_Server_Callbacks* callbacks);
TCP_Error TCP_Server_Add_Client(TCP_Server* server, TCP_Server_Client* client);
TCP_Error TCP_Server_Remove_Client(TCP_Server* server, TCP_Server_Client* client);
TCP_Error TCP_Server_Dispose(TCP_Server* server);

#endif