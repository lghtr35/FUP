#include "connection.hpp"

namespace fup
{
    // connection(int id);

    // void set_tcp(int socket_fd);
    // void set_udp(int socket_fd);
    int connection::send_message(int socket_fd, message *request)
    {
        int bytes_sent, message_len;
        uint8_t *buffer = request->serialize();
        message_len = request->size();
        if (message_len < 0)
        {
            fprintf(stderr, "Error in serializing message. error: %s\n", strerror(message_len));
            return (-1);
        }

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

    int connection::receive_message(int socket_fd, message *response)
    {
        if (response == NULL)
        {
            response = new message;
        }
        int fixed_message_size = response->header.size();
        int bytes_read = 0, bytes_parsed = 0;
        uint8_t fixed_message_buffer[fixed_message_size]; // VLA
        bytes_read = recv(socket_fd, fixed_message_buffer, fixed_message_size, 0);
        if (bytes_read == 0)
        {
            errno = ECONNRESET;
            fprintf(stderr, "server closed connection. error: %s\n", strerror(errno));
            return -1;
        }
        else if (bytes_read == -1)
        {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            return -1;
        }
        else if (bytes_read != fixed_message_size)
        {
            errno = ENODATA;
            fprintf(stderr, "server did not send message's fixed part. error: %s\n", strerror(errno));
            return -1;
        }

        bytes_parsed = response->header.deserialize(fixed_message_buffer);
        if (bytes_parsed == -1 || bytes_parsed != bytes_read)
        {
            errno = EPROTO;
            fprintf(stderr, "can not parse message's fixed part. error: %s\n", strerror(errno));
            return -1;
        }

        std::vector<uint8_t> data[response->size()]; // VLA
        memcpy(data, fixed_message_buffer, fixed_message_size);
        uint8_t buffer[BUFSIZ];
        int read_size = BUFSIZ, remaining_size = response->header.body_size, offset = fixed_message_size;
        bytes_read = 0;
        while (bytes_read >= 0 && bytes_read < response->header.body_size)
        {
            if (remaining_size < BUFSIZ)
            {
                read_size = remaining_size;
            }
            bytes_read = recv(socket_fd, buffer, read_size, 0);
            if (bytes_read == 0)
            {
                errno = ECONNRESET;
                fprintf(stderr, "server closed connection. error: %s\n", strerror(errno));
                return -1;
            }
            else if (bytes_read == -1)
            {
                errno = EPROTO;
                fprintf(stderr, "recv error: %s\n", strerror(errno));
                return -1;
            }
            memcpy(data + offset, buffer, bytes_read);
            offset += bytes_read;
            remaining_size -= bytes_read;
        }

        int status = response->deserialize(data);
        if (status < 0)
        {
            fprintf(stderr, "can not parse message's variable part. error: %s\n", strerror(errno));
            return -1;
        }

        return 0;
    }
}