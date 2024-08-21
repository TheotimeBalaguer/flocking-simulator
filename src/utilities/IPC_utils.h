#include <stdio.h>
#include <stdlib.h>

int openUDSSocketServer(const char *socket_path);

int acceptUDSSocketConnection(int server_fd);

void closeUDSSocket(int client_fd, int server_fd, const char* socket_path);

void sendOneMessage(int socket_fd, const char* message, int size);

void receiveOneMessage(int socket_fd, char* buffer);

void receiveOneMessageDoubleVector(int socket_fd, double* buffer);