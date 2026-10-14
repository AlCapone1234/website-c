#include "tcp_server.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>

TCP_Error TCP_Server_Initialize(uint16_t port)
{
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
        return TCP_Error_Bind;
    }

    if (listen(fd, 5) < 0)
    {
        return TCP_Error_Listen;
    }

    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_size);

    printf("Client connected\n");

    close(fd);
    close(client_fd);

    return TCP_Error_Success;
}