#ifndef TCP_ERROR_H
#define TCP_ERROR_H

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
    TCP_Error_Mutex,
    TCP_Error_Client_Could_Not_Be_Found,
} TCP_Error;


#endif