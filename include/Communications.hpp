#pragma once

#include "Socket.hpp"
#include "Message.hpp"

namespace fup
{
    class TcpWrapper
    {
    private:
        Version version;
        ConnectionId id;
        std::shared_ptr<Socket> socket;
        std::shared_ptr<SockAddr> destinationAddress;
    public:
        void Send(const Message& msg);
        Message Receive();
        Message SendAndReceive(const Message& msg);

        ConnectionId GetId() const { return id; }
        std::shared_ptr<Socket> GetSocket() const { return socket; }
        std::shared_ptr<SockAddr> GetLocalAddress() const;
        std::shared_ptr<SockAddr> GetDestinationAddress() const;


        TcpWrapper(Version v, std::string destinationUrl, std::string destinationPort);
        ~TcpWrapper() {};
    };

    class UdpWrapper {
    private:
        Version version;
        ConnectionId id;
        std::shared_ptr<Socket> socket;
    public:
        void Send(const Message& msg, const std::shared_ptr<SockAddr> dest);
        Message Receive();

        ConnectionId GetId() const { return id; }
        std::shared_ptr<Socket> GetSocket() const { return socket; }
        std::shared_ptr<SockAddr> GetLocalAddress() const;
        std::shared_ptr<SockAddr> GetDestinationAddress() const;

        UdpWrapper(Version version, fup::Port p = 0);
        ~UdpWrapper() {};
    };
} // namespace fup
