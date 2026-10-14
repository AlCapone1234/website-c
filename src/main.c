#include <stdio.h>

#include "tcp/tcp_server.h"

int main(void)
{
    TCP_Server_Initialize(1337);

    return 0;
}