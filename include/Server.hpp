#pragma once

#include "Common.hpp"
#include "Threadpool.hpp"
#include "Socket.hpp"

namespace fup
{
    class Server
    {
    public:
        Server(Version v, uint32_t maxConn = FUP_DEFAULT_SERVER_MAX_CONNECTION, uint16_t maxThreadPoolSize = FUP_DEFAULT_SERVER_MAX_THREAD);
        ~Server();

        void Run();
    private:
        Version version;
        uint32_t maxConnections;
        uint16_t maxThreadPoolSize;
        Socket listener;
        // std::unordered_map<ConnectionId, std::shared_ptr<Connection>> tcpConnections;
        // std::unordered_map<ConnectionId, std::shared_ptr<UdpConnection>> udpConnections;
        int maxSocketFd;
    };

    class Connection
    {
    public:
    private:
        
    };
}
