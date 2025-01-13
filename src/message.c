#include "message.h"

int _FUP_Message_Send(int socket_fd, const FUP_Message request)
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
int _FUP_Message_Receive(int socket_fd, FUP_Message *response)
{
    int fixed_message_size = FUP_Header_Size();
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

    bytes_parsed = FUP_Header_Serialize(&(response->header), fixed_message_buffer);
    if (bytes_parsed == -1 || bytes_parsed != bytes_read)
    {
        fprintf(stderr, "can not parse message's fixed part.\n");
        return (-4);
    }

    char data[response->header.size + fixed_message_size]; // VLA
    memcpy(data, fixed_message_buffer, fixed_message_size);
    char buffer[BUFSIZ];
    bytes_read = 0;
    int offset = fixed_message_size;
    while (bytes_read >= 0 && bytes_read < response->header.size)
    {
        bytes_read = recv(socket_fd, buffer, BUFSIZ, 0);
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
        memcpy(data + offset, buffer, bytes_read);
        offset += bytes_read;
    }

    int status = FUP_Message_Serialize(response, data);
    if (status < 0)
    {
        fprintf(stderr, "can not parse message's variable part. error: %s\n", strerror(errno));
        return (-5);
    }

    return 0;
}
