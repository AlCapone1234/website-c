#include "tcp_server_client.h"

#include <stdio.h>
#include <string.h>

TCP_Error TCP_Server_Client_Initialize(TCP_Server_Client* client)
{
    // Add error checking
    if (client == NULL)
    {
        // maybe better logging too?
        printf("%s", "[ERROR] TCP_Server_Client could not Initialize\n");
        return TCP_Error_Client_Is_NULL;
    }

    memset(client, 0, sizeof(TCP_Server_Client));
    client->addr_length = sizeof(client->addr);

    return TCP_Error_Success;
}