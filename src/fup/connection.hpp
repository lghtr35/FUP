#ifndef FUP_CONNECTION_HPP
#define FUP_CONNECTION_HPP
#include "message.hpp"

namespace fup
{
    struct connection
    {
    public:
        connection(int id);
        int send_message(int socket_fd, message *request);
        int receive_message(int socket_fd, message *response);
        void set_tcp(int socket_fd);
        void set_udp(int socket_fd);

    private:
        connection_id id;
        int tcp_socket_fd;
        int udp_socket_fd;
    };

}

#endif