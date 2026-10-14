#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdint.h>

typedef enum
{
    TCP_Error_Success = 0,
    TCP_Error_Socket,
    TCP_Error_Bind,
    TCP_Error_Listen
} TCP_Error;

typedef struct 
{

} TCP_Server;

TCP_Error TCP_Server_Initialize(uint16_t port);

#endif