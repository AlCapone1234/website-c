#include "tcp_server.h"

#include <stdio.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <fcntl.h>

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

    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    while (1)
    {
        int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_size);
        if (client_fd < 0) 
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                continue;
            perror("accept");
            break;
        }
        printf("Client connected\n");
        close(client_fd);
    }

    close(fd);

    return TCP_Error_Success;
}