#include "client.h"

FUP_Client FUP_Client_Init(int version, char *save_location)
{
    FUP_Client client = {.save_location = save_location, .version = (FUP_Version)version};

    return client;
}

void FUP_Client_Delete(FUP_Client *self)
{
    free(self->save_location);
    free(self);
}

// Kind of private/underlying functions
int _FUP_Client_Connect(FUP_Client *self, int ip_family, char *destination_url, unsigned short destination_port, FUP_Keyword message_type)
{
    int status;
    int socket_fd;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = ip_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (message_type == PACKET || message_type == CONNECT_UDP)
    {
        hints.ai_socktype = SOCK_DGRAM; // UDP
    }

    struct addrinfo *res;
    status = getaddrinfo(destination_url, destination_port, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "socket fd error: %s\n", gai_strerror(status));
    }

    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next) // loop over addrinfos to get a socket
    {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != -1)
        {
            break;
        }
        fprintf("can't connect socket: %s\n", strerror(socket_fd));
    }
    if (p == NULL || socket_fd == -1)
    {
        fprintf(stderr, "socket fd error: %s\n", strerror(socket_fd));
        return (-1);
    }
    status = connect(socket_fd, (struct sockaddr *)p, sizeof *p);
    if (status != 0)
    {
        fprintf(stderr, "connect error: %s\n", strerror(status));
        return (-1);
    }
    printf("connected socket to %s:%d\n", destination_url, destination_port);

    freeaddrinfo(res);
    return socket_fd;
}
int _FUP_Client_Disconnect(FUP_Client *self, int socket_fd)
{
    close(socket_fd);
    return 0;
}

// Operations possible from client
int FUP_Client_Upload(FUP_Client *self, char *destination_url, int destination_port, char *file_fullpath, int *res);
int FUP_Client_Download(FUP_Client *self, char *destination_url, int destination_port, char *filename, int *res);
int FUP_Client_ListFiles(FUP_Client *self, char *destination_url, int destination_port, char **res);