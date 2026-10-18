#include <stdio.h>

#include "tcp/tcp_server.h"
#include <pthread.h>
#include <unistd.h>

int main(void)
{
    TCP_Server server = {0};

    TCP_Error err = TCP_Server_Initialize(&server, 1337, NULL);
    printf("TCP_Server_Initialize returned with %d\n", err);
    
    TCP_Server_Dispose(&server);

    return 0;
}