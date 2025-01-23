#ifndef FUP_CONNECTION_HPP
#define FUP_CONNECTION_HPP
#include "Message.hpp"

namespace fup
{
    class Connection
    {
    public:
        Connection(Version v, int socket_fd);
        Message server_accept();
        // shared
        ConnectionId id;
        bool getIsConnected();
        ~Connection();
        Message sendAndReceiveMessage(Message& request);
    private:
        void disconnect();
        void sendMessage(Message& request);
        Message receiveMessage();
        bool isConnected;
        int tcpSocketfd;
        Version version;
        std::mutex mutex;
    };
}

#endif