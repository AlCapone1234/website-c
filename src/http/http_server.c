#include "http_server.h"


/*
HTTP_Error HTTP_Server_Initialize(HTTP_Server* server, uint16_t port, HTTP_Server_On_Initialized on_initialize)
{
    if (server == NULL)
    {
        return HTTP_Error_Server;
    }

    pthread_mutex_init(&server->mutex, NULL);

    TCP_Error tcp_err = TCP_Server_Initialize(&server->tcp_server, port, on_initialize);
    if (tcp_err != TCP_Error_Success)
    {
        return HTTP_Error_TCP;
    }

    return HTTP_Error_Success;
}
*/

HTTP_Error HTTP_Server_Dispose(HTTP_Server* server)
{
    if (server == NULL)
    {
        return HTTP_Error_Server;
    }

    TCP_Server_Dispose(&server->tcp_server);

    if (pthread_mutex_destroy(&server->mutex) != 0)
    {
        return HTTP_Error_Mutex;
    }

    return HTTP_Error_Success;
}