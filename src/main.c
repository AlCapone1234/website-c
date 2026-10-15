#include <stdio.h>

#include "tcp/tcp_server.h"

int main(void)
{
    TCP_Server server = {0};
    TCP_Server_Initialize(&server, 1337);

    return 0;
}