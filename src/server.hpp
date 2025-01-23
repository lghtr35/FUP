#ifndef FUP_SERVER_HPP
#define FUP_SERVER_HPP

#include "Common.hpp"
#include "Connection.hpp"
#include "Threadpool.hpp"
#define DEFAULT_TCP_PORT 12420
#define DEFAULT_UDP_PORT 12421

namespace fup
{
    class Server
    {
    public:
        Server(unsigned int v, int maxConn, std::string sl);
        ~Server();

        void Listen();
    private:
        void listenTcp();
        void listenUdp();
        Version version;
        int maxConnections;
        int currentConnections;
        std::vector<Connection> connections;
        int tcpSocket; // id for socket acquired from socket()
        int udpSocket; // id for socket acquired from socket()
        std::string saveLocation;
        std::vector<std::string> filenames;
        std::unique_ptr<Threadpool> threads;
    };
}
#endif