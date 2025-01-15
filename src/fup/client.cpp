#include "client.hpp"

Client Init(int version, char *save_location)
{
    Client client = {.save_location = save_location, .version = (Version)version};

    return client;
}

void Delete(Client *self)
{
    free(self->save_location);
    free(self);
}

// Kind of private/underlying functions
int _Connect(Client *self, char *destination_url, unsigned short destination_port, int sock_type)
{
    int status;
    int socket_fd;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = sock_type;
    hints.ai_flags = AI_PASSIVE;

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
void _Disconnect(Client *self, int socket_fd)
{
    close(socket_fd);
}

// Operations possible from client
int Upload(Client *self, char *destination_url, int destination_port, char *file_fullpath, int *res)
{

    return 0;
}
int Download(Client *self, char *destination_url, int destination_port, char *filename, int *res)
{
    int tcp_socket_fd = _Connect(self, destination_url, destination_port, SOCK_STREAM);
    if (tcp_socket_fd < 0)
    {
        return -1;
    }

    Message request = {.header = {.version = self->version, .keyword = DOWNLOAD, .body_size = 0}};
    M_Download download_body = {.filename_size = strlen(filename), .filename = filename};
    request.body = &download_body;
    request.header.body_size = M_Download_Size(&download_body);
    int status = Message_Send(tcp_socket_fd, request);
    if (status < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        return -2;
    }

    Message response;
    status = Message_Receive(tcp_socket_fd, &response);
    if (status < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        return -3;
    }

    if (response.header.keyword == ABORT)
    {
        _Disconnect(self, tcp_socket_fd);
        errno = ECONNABORTED;
        fprintf(stderr, "server aborted the download. error: %s\n", strerror(errno));
        return -4;
    }

    if (response.header.keyword != ACK || response.header.connection_id)
    {
        _Disconnect(self, tcp_socket_fd);
        errno = EPROTO;
        fprintf(stderr, "unexpected response from server. error: %s\n", strerror(errno));
        return -5;
    }

    int udp_socket_fd = _Connect(self, destination_url, destination_port, SOCK_DGRAM);
    if (udp_socket_fd < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        return -6;
    }

    request.header.keyword = CONNECT_UDP;
    request.header.body_size = 0;
    request.body = NULL;
    status = Message_Send(udp_socket_fd, request);
    if (status < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        return -7;
    }

    status = Message_Receive(udp_socket_fd, &response);
    if (status < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        return -8;
    }

    if (response.header.keyword == ABORT)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        errno = ECONNABORTED;
        fprintf(stderr, "server aborted the download. error: %s\n", strerror(errno));
        return -9;
    }

    if (response.header.keyword != ACK || response.header.connection_id)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        errno = EPROTO;
        fprintf(stderr, "unexpected response from server. error: %s\n", strerror(errno));
        return -10;
    }

    request.header.keyword = ACK;
    status = Message_Send(udp_socket_fd, request);
    if (status < 0)
    {
        _Disconnect(self, tcp_socket_fd);
        _Disconnect(self, udp_socket_fd);
        return -11;
    }

    return 0;
}
int ListFiles(Client *self, char *destination_url, int destination_port, char **res);