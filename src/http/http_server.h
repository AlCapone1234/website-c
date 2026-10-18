#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "../tcp/tcp_server.h"

typedef enum
{
    HTTP_Error_Success = 0,
    HTTP_Error_Server,
    HTTP_Error_TCP,
    HTTP_Error_Mutex,
} HTTP_Error;

typedef struct
{
    TCP_Server tcp_server;
    pthread_mutex_t mutex;
} HTTP_Server;

typedef TCP_Server_On_Initialized HTTP_Server_On_Initialized;

//HTTP_Error HTTP_Server_Initialize(HTTP_Server* server, uint16_t port, HTTP_Server_On_Initialized on_initialize);
HTTP_Error HTTP_Server_Dispose(HTTP_Server* server);


#endif