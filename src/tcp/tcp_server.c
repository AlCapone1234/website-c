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
#include <poll.h>


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
    if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
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

    while (1)
    {
        struct pollfd pollfds[server->clients_length + 1];
        pollfds[0].fd = fd;
        pollfds[0].events = POLLIN;

        int nfds = 1;
        for (size_t i = 0; i < server->clients_length; i++)
        {
            pollfds[nfds].fd = server->clients[i].fd;
            pollfds[nfds].events = POLLIN;
            pollfds[nfds].revents = 0;
            nfds++;
        }

        int ready = poll(pollfds, nfds, 100);
        if (ready <= 0)
            continue;


        if (pollfds[0].revents & POLLIN)
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

            int client_flags = fcntl(client_fd, F_GETFL, 0);
            if (client_flags < 0 || fcntl(client_fd, F_SETFL, client_flags | O_NONBLOCK) < 0)
            {
                close(client_fd);
                return TCP_Error_Fcntl;
            }

            TCP_Error add_result = TCP_Server_Add_Client(server, &client);
            if (add_result == TCP_Error_Malloc || add_result == TCP_Error_Realloc)
            {
                // should probably do something else here.
                close(fd);
                close(client_fd);
                return add_result;
            }

        }

        for (size_t i = 0; i < server->clients_length; i++)
        {
            TCP_Server_Client* client = &server->clients[i];

            int pfd = -1;
            for (size_t j = 0; j < server->clients_length + 1; j++)
                if (pollfds[j].fd == client->fd)
                    pfd = j;

            if (pfd < 0)
                continue;

            if (pollfds[pfd].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                TCP_Server_Remove_Client(server, client);
                i--;
                continue;
            }

            if (pollfds[pfd].revents & POLLIN)
            {
                ssize_t bytes = recv(client->fd, client->outgoing_buffer, TCP_SERVER_CLIENT_MAX_BUFFER - 1, 0);

                if (bytes > 0)
                {
                    client->outgoing_buffer[bytes] = '\0';
                    printf("Received from fd %d: %s\n", client->fd, client->outgoing_buffer);
                }
                else if (bytes == 0)
                {
                    // orderly shutdown
                    printf("Client fd %d disconnected\n", client->fd);
                    TCP_Server_Remove_Client(server, client);
                    i--;
                    continue;
                }
                else
                {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        perror("recv");
                        TCP_Server_Remove_Client(server, client);
                        i--;
                        continue;
                    }
                }
            }

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