#ifndef FUP_SERVER_HPP
#define FUP_SERVER_HPP

#include "common.hpp"
#include "message.hpp"
#define DEFAULT_TCP_PORT 12420
#define DEFAULT_UDP_PORT 12421

namespace fup
{
    class Server
    {
    public:
        Server(int version, int max_conn_count, char *save_location);
        ~Server();

        void Server_Listen();

    private:
        version version;
        int max_connection_count;
        int connection_count;
        connection_id *connections;
        int tcp_socket; // id for socket acquired from socket()
        int file_count;
        char *save_location;
        char **filenames;
    };
}
#endif