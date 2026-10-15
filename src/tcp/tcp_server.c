#include "tcp_server.h"
#include "tcp_server_client.h"

#include <stdio.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

TCP_Error TCP_Server_Initialize(TCP_Server* server, uint16_t port)
{
    if (server == NULL)
    {
        return TCP_Error_Server_Is_NULL;
    }

    server->clients = NULL;
    server->clients_length = 0;
    server->clients_capacity = TCP_SERVER_CLIENTS_CAPACITY_START; 

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return TCP_Error_Socket;
    }

    struct sockaddr_in server_addr; 

    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(port); 

    if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        close(fd);
        return TCP_Error_Bind;
    }

    if (listen(fd, 5) < 0)
    {
        close(fd);
        return TCP_Error_Listen;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
    {
        close(fd);
        return TCP_Error_Fcntl;
    }

    TCP_Server_Client client = {0};
    TCP_Server_Client_Initialize(&client);


    while (1)
    {
        int client_fd = accept(fd, (struct sockaddr*)&client.addr, &client.addr_length);
        client.fd = client_fd;

        if (client_fd < 0) 
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                continue;
            perror("accept");
            break;
        }
        //printf("Client connected\n");
        //close(client_fd);

        TCP_Server_Add_Client(server, &client);
    }

    close(fd);
    return TCP_Error_Success;
}

TCP_Error TCP_Server_Add_Client(TCP_Server* server, TCP_Server_Client* client)
{
    if (server == NULL)
        return TCP_Error_Server_Is_NULL;

    if (client == NULL)
        return TCP_Error_Client_Is_NULL;

    
    if (server->clients == NULL)
    {
        server->clients = (TCP_Server_Client*)malloc(sizeof(TCP_Server_Client) * TCP_SERVER_CLIENTS_CAPACITY_START);
        if (server->clients == NULL)
        {
            return TCP_Error_Malloc;
        }
        
        memcpy(&server->clients[server->clients_length++], client, sizeof(TCP_Server_Client));
    }
    else if (server->clients_length >= server->clients_capacity)
    {
        if (server->clients_capacity < 1)
            return TCP_Error_Clients_Capacity_Is_Less_Than_One;

        server->clients_capacity *= 2;
        
        server->clients = (TCP_Server_Client*)realloc(server->clients, sizeof(TCP_Server_Client) * server->clients_capacity);
        if (server->clients == NULL)
        {
            return TCP_Error_Realloc;
        }

        memcpy(&server->clients[server->clients_length++], client, sizeof(TCP_Server_Client));
    }
    else
    {
        memcpy(&server->clients[server->clients_length++], client, sizeof(TCP_Server_Client));
    }

    printf("Current clients: %ld\n", server->clients_length);

    return TCP_Error_Success;
}