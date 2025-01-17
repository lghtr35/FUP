#ifndef FUP_CONNECTION_HPP
#define FUP_CONNECTION_HPP
#include "message.hpp"

namespace fup
{
    struct connection
    {
    public:        
        connection_id id;
        void send_message(std::shared_ptr<message> request, bool is_tcp);
        message receive_message(bool is_tcp);
        void receive_message_threaded(bool is_tcp, std::shared_ptr<message> message);
        void set_tcp(int socket_fd);
        void set_udp(int socket_fd);
        ~connection();
    private:
        int tcp_socket_fd;
        int udp_socket_fd;
    };

}

#endif