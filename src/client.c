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
int _FUP_Client_Send(FUP_Client *self, int socket_fd, const FUP_Message request)
{
    int bytes_sent, message_len;
    char *buffer;
    message_len = FUP_Message_Serialize(&request, buffer);

    bytes_sent = send(socket_fd, buffer, message_len, 0);
    if (bytes_sent == -1)
    {
        fprintf(stderr, "send error: %s\n", strerror(errno));
        return (-1);
    }
    else if (bytes_sent == message_len)
    {
        printf("sent full message: \"%s\"\n", buffer);
    }
    else
    {
        printf("sent partial message: %d bytes sent.\n", bytes_sent);
        return (-2);
    }

    return 0;
}
int _FUP_Client_Receive(FUP_Client *self, int socket_fd, FUP_Message *response)
{
    int fixed_message_size = sizeof(unsigned char[16]) + (2 * sizeof(unsigned int)) + sizeof(FUP_Keyword);
    int bytes_read = 0, bytes_parsed = 0;
    char fixed_message_buffer[fixed_message_size]; // VLA
    bytes_read = recv(socket_fd, fixed_message_buffer, fixed_message_size, 0);
    if (bytes_read == 0)
    {
        fprintf(stderr, "server closed connection.\n");
        return (-2);
    }
    else if (bytes_read == -1)
    {
        fprintf(stderr, "recv error: %s\n", strerror(errno));
        return (-1);
    }
    else if (bytes_read != fixed_message_size)
    {
        fprintf(stderr, "server did not send message's fixed part.\n");
        return (-3);
    }

    bytes_parsed = FUP_Message_Serialize_Fixed(response, fixed_message_buffer);
    if (bytes_parsed == -1 || bytes_parsed != bytes_read)
    {
        fprintf(stderr, "can not parse message's fixed part.\n");
        return (-4);
    }

    char data[response->size + fixed_message_size]; // VLA
    char buffer[BUFSIZ];
    bytes_read = 0;
    int offset = 0;
    while (bytes_read >= 0)
    {
        bytes_read = recv(socket_fd, variable_buffer, BUFSIZ, 0);
        if (bytes_read == 0)
        {
            fprintf(stderr, "server closed connection.\n");
            break;
        }
        else if (bytes_read == -1)
        {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            break;
        }
    }
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