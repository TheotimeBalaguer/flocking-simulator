#include "IPC_utils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int openUDSSocketServer(const char *socket_path)
{
    int server_fd;
    struct sockaddr_un addr;

    // Create the socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    // Zero out the address struct
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    // Unlink the file if it already exists
    unlink(socket_path);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind error");
        exit(-1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }

    return server_fd;
}

int acceptUDSSocketConnection(int server_fd)
{
    int client_fd;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Accept a connection
    if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        perror("accept error");
        exit(-1);
    }

    printf("Client connected\n");

    return client_fd;
}

void closeUDSSocket(int client_fd, int server_fd, const char* socket_path)
{
    // Cleanup
    close(client_fd);
    close(server_fd);
    unlink(socket_path);
}

void sendOneMessage(int socket_fd, const char* message, int size)
{
    // Send preamble
    if(write(socket_fd, &size, 4) != 4)
    {
        perror("write error (preamble)");
        exit(-1);
    }
    // Send message
    if(write(socket_fd, message, size) != size)
    {
        perror("write error (message)");
        exit(-1);
    }
}

void receiveOneMessage(int socket_fd, char* buffer)
{
    // Read preamble
    unsigned int size;
    if(read(socket_fd, &size, 4) == -1)
    {
        perror("read error (preamble)");
        exit(-1);
    }
    
    // Read message
    if(read(socket_fd, buffer, size) == -1)
    {
        perror("read error (message)");
        exit(-1);
    }
}

void receiveOneMessageDoubleVector(int socket_fd, double* buffer)
{
    // Read preamble
    unsigned int size;
    if(read(socket_fd, &size, 4) == -1)
    {
        perror("read error (preamble)");
        exit(-1);
    }
    size /= sizeof(double);
    // Read message
    if(read(socket_fd, buffer, size) == -1)
    {
        perror("read error (message)");
        exit(-1);
    }
}