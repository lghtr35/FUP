#include "connection.hpp"

namespace fup
{
    connection::connection() : udp_socket_fd(-1), tcp_socket_fd(-1), is_connected(false) {}
    connection::~connection() {
        close(udp_socket_fd);
        close(tcp_socket_fd);
    }

    bool connection::get_is_connected() {
        return is_connected;
    }

    void connection::set_tcp(int socket_fd) {
        this->tcp_socket_fd = socket_fd;
    }

    void connection::set_udp(int socket_fd) {
        this->udp_socket_fd = socket_fd;
    }

    void connection::send_message(std::shared_ptr<message> request, bool is_tcp)
    {
        int socket_fd = is_tcp ? this->tcp_socket_fd : this->udp_socket_fd;
        int bytes_sent, message_len;
        std::vector<uint8_t> buffer = request->serialize();
        message_len = request->size();
        if (message_len < 0)
        {
            throw std::runtime_error("Error in serializing message.");
        }
        {
            std::unique_lock<std::mutex> lock(this->mutex);
            bytes_sent = send(socket_fd, buffer.data(), message_len, 0);
            if (bytes_sent == -1)
            {
                std::ostringstream s;
                s << "error: " << strerror(errno) << "\n ";
                throw std::runtime_error(s.str());
            }
            else if (bytes_sent != message_len)
            {
                errno = EPROTO;
                std::ostringstream s;
                s << "sent partial message with" << bytes_sent << " bytes sent. error: " << strerror(errno) << "\n";
                throw std::runtime_error(s.str());
            }
        }
    }

    void connection::receive_message_threaded(bool is_tcp, std::shared_ptr<message> message) {
        *message = this->receive_message(is_tcp);
    }

    message connection::receive_message(bool is_tcp)
    {
        message response;
        int socket_fd = is_tcp ? this->tcp_socket_fd : this->udp_socket_fd;
        int fixed_message_size = response.header.size();
        int bytes_read = 0, bytes_parsed = 0;
        std::vector<uint8_t> fixed_message_buffer(fixed_message_size); // VLA
        {
            std::unique_lock<std::mutex> lock(this->mutex);
            bytes_read = recv(socket_fd, fixed_message_buffer.data(), fixed_message_size, 0);
            if (bytes_read == 0)
            {
                errno = ECONNRESET;
                std::ostringstream s;
                s << "server closed connection. error: " << strerror(errno) << "\n";
                throw std::runtime_error(s.str());
            }
            else if (bytes_read == -1)
            {
                std::ostringstream s;
                s << "recv error: " << strerror(errno) << "\n";
                throw std::runtime_error(s.str());
            }
            else if (bytes_read != fixed_message_size)
            {
                errno = EPROTO;
                std::ostringstream s;
                s << "server did not send message header part. error: " << strerror(errno) << "\n";
                throw std::runtime_error(s.str());
            }

            bytes_parsed = response.header.deserialize(fixed_message_buffer);
            if (bytes_parsed == -1 || bytes_parsed != bytes_read)
            {
                std::ostringstream s;
                s << "can not parse message header. error: " << strerror(errno) << "\n";
                throw std::runtime_error(s.str());
            }

            response.body.resize(response.header.body_size); // VLA
            uint8_t buffer[BUFSIZ];
            int read_size = BUFSIZ, remaining_size = response.header.body_size, offset = 0;
            bytes_read = 0;
            while (bytes_read >= 0 && bytes_read < response.header.body_size)
            {
                if (remaining_size < BUFSIZ)
                {
                    read_size = remaining_size;
                }
                bytes_read = recv(socket_fd, buffer, read_size, 0);
                if (bytes_read == 0)
                {
                    errno = ECONNRESET;
                    std::ostringstream s;
                    s << "server closed connection. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }
                else if (bytes_read == -1)
                {
                    errno = EPROTO;
                    std::ostringstream s;
                    s << "recv error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }
                memcpy(response.body.data() + offset, buffer, bytes_read);
                offset += bytes_read;
                remaining_size -= bytes_read;
            }
        }

        return response;
    }
}