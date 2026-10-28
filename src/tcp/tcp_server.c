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

#define MAX_BUFFER 4096

void* clients_loop(void* arg)
{
    TCP_Server* server = (TCP_Server*)arg;

    while (1)
    {
        pthread_mutex_lock(&server->mutex);
        for (size_t i = 0; i < server->clients_length; i++)
        {
            ssize_t total_bytes = 0;
            char buffer[MAX_BUFFER];

            while (1)
            {
                if (MAX_BUFFER - total_bytes <= 0)
                {
                    break;
                }

                ssize_t bytes_read = recv(server->clients[i].fd, &buffer[total_bytes], MAX_BUFFER - total_bytes, 0);

                if (bytes_read > 0)
                {
                    total_bytes += bytes_read;
                }
                else if (bytes_read < 0)
                {
                    break;
                }
                else if (bytes_read == 0)
                {
                    if (total_bytes == 0)
                        break;

                    buffer[total_bytes] = '\0';
                    printf("Reading: [%s]\n", buffer);

                    TCP_Error result = TCP_Server_Remove_Client(server, &server->clients[i]);
                    if (result != TCP_Error_Success)
                        printf("Failed to remove TCP_Server_Client\n");
                }
            }

        }
        pthread_mutex_unlock(&server->mutex);
    }
}

TCP_Error TCP_Server_Initialize(TCP_Server* server, uint16_t port, TCP_Server_Callbacks* callbacks)
{
    if (server == NULL)
    {
        return TCP_Error_Server_Is_NULL;
    }

    server->clients = NULL;
    server->clients_length = 0;
    server->clients_capacity = TCP_SERVER_CLIENTS_CAPACITY_START; 
    server->callbacks = callbacks;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return TCP_Error_Socket;
    }

    struct sockaddr_in server_addr = {0}; 

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
    if (flags < 0)
    {
        close(fd);
        return TCP_Error_Fcntl;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
    {
        close(fd);
        return TCP_Error_Fcntl;
    }

    TCP_Server_Client client = {0};

    TCP_Error client_err = TCP_Server_Client_Initialize(&client);
    if (client_err != TCP_Error_Success)
    {
        close(fd);        
        return client_err;
    }

    if (callbacks != NULL && callbacks->on_initialized != NULL)
        callbacks->on_initialized(server);

    pthread_mutex_init(&server->mutex, NULL);

    pthread_t clients_thread;
    pthread_create(&clients_thread, NULL, clients_loop, server);
    pthread_detach(clients_thread);

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

        TCP_Error add_result = TCP_Server_Add_Client(server, &client);
        if (add_result == TCP_Error_Malloc || add_result == TCP_Error_Realloc)
        {
            // should probably do something else here.
            close(fd);
            close(client_fd);
            return add_result;
        }
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
        {
            return TCP_Error_Clients_Capacity_Is_Less_Than_One;
        }

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

    //printf("Current clients: %ld\n", server->clients_length);

    return TCP_Error_Success;
}

TCP_Error TCP_Server_Dispose(TCP_Server* server)
{
    if (server == NULL)
        return TCP_Error_Server_Is_NULL;

    if (server->clients != NULL)
    {
        for (size_t i = 0; i < server->clients_length; i++)
        {
            close(server->clients[i].fd);
        }

        free(server->clients);
        server->clients = NULL;
    }

    if (server->callbacks->on_shutdown != NULL)
        server->callbacks->on_shutdown(server);

    return TCP_Error_Success;
}

TCP_Error TCP_Server_Remove_Client(TCP_Server* server, TCP_Server_Client* client)
{
    if (server == NULL)
        return TCP_Error_Server_Is_NULL;

    if (client == NULL)
        return TCP_Error_Client_Is_NULL;


    for (size_t i = 0; i < server->clients_length; i++)
    {
        if (&server->clients[i] == client)
        {
            close(server->clients[i].fd);
            server->clients[i] = server->clients[server->clients_length - 1];
            memset(&server->clients[server->clients_length - 1], 0, sizeof(TCP_Server_Client));
            server->clients_length--;

            return TCP_Error_Success;
        }
    }

    return TCP_Error_Client_Could_Not_Be_Found;
}