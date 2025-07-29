#include "Socket.hpp"

namespace fup
{
    int Socket::Send(const void* buffer, int length, std::shared_ptr<SockAddr> dest)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (dest != nullptr) {
            destAddr = dest;
        }
        try {
            if (!isTcp) {
                if (destAddr == nullptr && dest == nullptr) {
                    throw std::runtime_error("Socket::Send: Can not send an UDP message without address info");
                }
                return sendToBase(buffer, length);
            }

            return sendBase(buffer, length);
        }
        catch (std::exception& e) {
            throw std::runtime_error("Socket::Send: An error occured when sending a message: " + std::string(e.what()));
        }
    }

    int Socket::sendToBase(const void* buffer, int length)
    {
        int bytesSent = sendto(handle, buffer, length, 0, (sockaddr*)(destAddr->GetAddr()), destAddr->GetAddrLen());
        if (bytesSent == -1) {
            throw std::runtime_error("Socket::sendToBase: An error occured while sending UDP message: " + std::string(strerror(errno)));
        }
        return bytesSent;
    }

    int Socket::sendBase(const void* buffer, int length) {
        int bytesSent = 0;
        while (bytesSent < length) {
            int currentBytesSent = send(handle, (const char*)buffer + bytesSent, length - bytesSent, 0);
            if (currentBytesSent == -1) {
                throw std::runtime_error("Socket::sendBase: An error occured while sending TCP message: " + std::string(strerror(errno)));
            }
            bytesSent += currentBytesSent;
        }

        return bytesSent;
    }

    int Socket::Receive(void* buffer, size_t length)
    {
        struct sockaddr_storage addrStorage;
        socklen_t addrlen = sizeof(addrStorage);
        std::lock_guard<std::mutex> lock(mutex);
        int bytesRead;
        if (!isTcp) {
            bytesRead = recvfrom(handle, buffer, length, 0, (sockaddr*)&addrStorage, &addrlen);
            if (bytesRead == -1) {
                throw std::runtime_error("Socket::Receive: An error occured while receiving UDP message: " + std::string(strerror(errno)));
            }

            if (destAddr == nullptr) {
                destAddr = std::make_shared<SockAddr>(&addrStorage, addrlen);
            }

            return bytesRead;
        }

        bytesRead = recv(handle, buffer, length, 0);
        if (bytesRead == -1) {
            throw std::runtime_error("Socket::Receive: An error occured while receiving TCP message: " + std::string(strerror(errno)));
        }
        else if (bytesRead == 0) {
            throw std::runtime_error("Socket::Receive: Peer closed connection while receiving TCP message");
        }

        return bytesRead;
    }

    void Socket::Bind(const sockaddr* addr, socklen_t addrlen)
    {
        std::lock_guard<std::mutex> lock(mutex);
        int status = bind(handle, addr, addrlen);
        if (status == -1) {
            throw std::runtime_error("Socket::Bind: An error occured while binding socket handler: " + std::string(strerror(errno)));
        }
    }

    void Socket::Listen(int backlog)
    {
        std::lock_guard<std::mutex> lock(mutex);
        int status = listen(handle, backlog);
        if (status == -1) {
            throw std::runtime_error("Socket::Listen: An error occured while listening: " + std::string(strerror(errno)));
        }
    }

    Socket Socket::Accept()
    {
        struct sockaddr_storage addrStorage;
        socklen_t addrlen = sizeof(addrStorage);
        std::lock_guard<std::mutex> lock(mutex);
        int nHandle = accept(handle, (sockaddr*)&addrStorage, &addrlen);
        if (nHandle == -1) {
            throw std::runtime_error("Socket::Accept: An error occured while accepting a connection: " + std::string(strerror(errno)));
        }

        auto nDestAddr = std::make_shared<SockAddr>(&addrStorage, addrlen);
        return Socket(isTcp, nHandle, localAddr, nDestAddr);
    }

    void Socket::Close()
    {
        std::lock_guard<std::mutex> lock(mutex);
        int status = socket_close(handle);
        if (status == -1) {
            throw std::runtime_error("Socket::Close: An error occured while closing the socket: " + std::string(strerror(errno)));
        }
    }

    Socket::Socket(bool i, Port port) {
        isTcp = i;
        struct addrinfo hints, * res, * p;
        SocketHandle sockfd;
        int err, socktype = isTcp ? SOCK_STREAM : SOCK_DGRAM;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = socktype;
        hints.ai_flags = AI_PASSIVE;

        std::string portStr = std::to_string(port);
        err = getaddrinfo(NULL, portStr.c_str(), &hints, &res);
        if (err != 0) {
            throw std::runtime_error("sock::getSock: An error occured in getaddrinfo: " + std::string(gai_strerror(err)));
        }

        for (p = res; p != NULL; p = p->ai_next) {
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sockfd == INVALID_SOCKET_HANDLE) {
                continue;
            }
            break;
        }

        if (p == NULL) {
            freeaddrinfo(res);
            throw std::runtime_error("sock::getSock: Failed to create socket");
        }

        handle = sockfd;
        sockaddr_storage storage;
        memset(&storage, 0, sizeof(storage));
        memcpy(&storage, p->ai_addr, p->ai_addrlen);
        localAddr = std::make_shared<SockAddr>(&storage, p->ai_addrlen);

        freeaddrinfo(res);
    }

    Socket::Socket(bool i, SocketHandle sockfd, std::shared_ptr<SockAddr> local, std::shared_ptr<SockAddr> dest) {
        handle = sockfd;
        isTcp = i;
        localAddr = local;
        destAddr = dest;
    }

    bool Socket::IsBlocking() const {
        return isBlocking;
    }

    void Socket::SetBlocking(bool blocking) {
        std::lock_guard<std::mutex> lock(mutex);
        if (isBlocking == blocking) return;
#ifdef _WIN32
        if (blocking) {
            u_long mode = 0;
            if (ioctlsocket(handle, FIONBIO, &mode) != 0) {
                throw std::runtime_error("Socket::Socket: Failed to set socket blocking mode: " + std::to_string(WSAGetLastError()));
            }
        }
        else {
            u_long mode = 1;
            if (ioctlsocket(handle, FIONBIO, &mode) != 0) {
                throw std::runtime_error("Socket::Socket: Failed to set socket non-blocking mode: " + std::to_string(WSAGetLastError()));
            }
        }
#else
        if (blocking) {
            int flags = fcntl(handle, F_GETFL, 0);
            flags &= ~O_NONBLOCK;
            int status = fcntl(handle, F_SETFL, flags);
            if (status == -1) {
                throw std::runtime_error("Socket::Socket: Failed to set socket blocking mode: " + std::string(strerror(errno)));
            }
        }
        else {
            // Set the socket to non-blocking mode
            int flags = fcntl(handle, F_GETFL, 0);
            flags |= O_NONBLOCK;
            int status = fcntl(handle, F_SETFL, flags);
            if (status == -1) {
                throw std::runtime_error("Socket::Socket: Failed to set socket blocking mode: " + std::string(strerror(errno)));
            }
        }
#endif        
        isBlocking = blocking;
    }

    bool Socket::IsTcp() const {
        return isTcp;
    }

    std::shared_ptr<SockAddr> Socket::GetLocalAddr() {
        return localAddr;
    }

    std::shared_ptr<SockAddr> Socket::GetDestinationAddr() {
        return destAddr;
    }

    void Socket::SetTimeout(int timeoutMs)
    {
        int secs = timeoutMs / 1000;
        int usecs = (timeoutMs % 1000) * 1000;
        struct timeval timeout;
        timeout.tv_sec = secs;
        timeout.tv_usec = usecs;

        std::lock_guard<std::mutex> lock(mutex);
        if (setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
            throw std::runtime_error("Socket::SetTimeout: Failed to set socket receive timeout: " + std::string(strerror(errno)));
        }
        if (setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
            throw std::runtime_error("Socket::SetTimeout: Failed to set socket send timeout: " + std::string(strerror(errno)));
        }
    }

    void Socket::Connect(const std::string& url, const std::string& port)
    {
        std::lock_guard<std::mutex> lock(mutex);
        struct addrinfo hints, * res, * p;
        int err;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = isTcp ? SOCK_STREAM : SOCK_DGRAM;
        hints.ai_flags = 0;

        err = getaddrinfo(url.c_str(), port.c_str(), &hints, &res);
        if (err != 0) {
            throw std::runtime_error("Socket::Connect: An error occured on getaddrinfo: " + std::string(gai_strerror(err)));
        }

        SockAddr sockAddr;
        for (p = res; p != NULL; p = p->ai_next) {
            int status = connect(handle, p->ai_addr, p->ai_addrlen);
            if (status != -1) {
                break;
            }
        }

        if (p == NULL) {
            throw std::runtime_error("Socket::Connect: An error occured while conneting: " + std::string(gai_strerror(err)));
        }

        freeaddrinfo(res);
        destAddr = std::make_shared<SockAddr>(sockAddr);
    }

    Port SockAddr::GetPort() {
        if (!addrBuf || addrLen < sizeof(sa_family_t)) {
            throw std::runtime_error("SockAddr::GetPort: address is not set");
        }

        const struct sockaddr* sa = reinterpret_cast<const struct sockaddr*>(addrBuf.get());

        if (sa->sa_family == AF_INET) {
            if (addrLen < sizeof(struct sockaddr_in)) {
                throw std::runtime_error("SockAddr::GetPort: address does not contain enough data for corresponding family: IPv4");
            }
            const struct sockaddr_in* sin = reinterpret_cast<const struct sockaddr_in*>(sa);
            return ntohs(sin->sin_port);
        }
        else if (sa->sa_family == AF_INET6) {
            if (addrLen < sizeof(struct sockaddr_in6)) {
                throw std::runtime_error("SockAddr::GetPort: address does not contain enough data for corresponding family: IPv6");
            }
            const struct sockaddr_in6* sin6 = reinterpret_cast<const struct sockaddr_in6*>(sa);
            return ntohs(sin6->sin6_port);
        }
        else {
            throw std::runtime_error("SockAddr::GetPort: address is not an IPv4 or IPv6 type");
        }
    }

    std::string SockAddr::GetIpAddress() {
        if (!addrBuf || addrLen < sizeof(sa_family_t)) {
            throw std::runtime_error("SockAddr::GetIpAddress: address is not set");
        }

        const struct sockaddr* sa = reinterpret_cast<const struct sockaddr*>(addrBuf.get());
        char ip_str[INET6_ADDRSTRLEN];

        if (sa->sa_family == AF_INET) {
            if (addrLen < sizeof(struct sockaddr_in)) {
                throw std::runtime_error("SockAddr::GetIpAddress: address does not contain enough data for corresponding family: IPv4");
            }
            const struct sockaddr_in* sin = reinterpret_cast<const struct sockaddr_in*>(sa);
            if (inet_ntop(AF_INET, &(sin->sin_addr), ip_str, INET_ADDRSTRLEN) == nullptr) {
                throw std::runtime_error(std::string("SockAddr::GetIpAddress: Failed to convert IPv4 address to string: ") + strerror(errno));
            }
        }
        else if (sa->sa_family == AF_INET6) {
            if (addrLen < sizeof(struct sockaddr_in6)) {
                throw std::runtime_error("SockAddr::GetIpAddress: address does not contain enough data for corresponding family: IPv6");
            }
            const struct sockaddr_in6* sin6 = reinterpret_cast<const struct sockaddr_in6*>(sa);
            if (inet_ntop(AF_INET6, &(sin6->sin6_addr), ip_str, INET6_ADDRSTRLEN) == nullptr) {
                throw std::runtime_error(std::string("SockAddr::GetIpAddress: Failed to convert IPv6 address to string: ") + strerror(errno));
            }
        }
        else {
            throw std::runtime_error("SockAddr::GetIpAddress: address is not an IPv4 or IPv6 type");
        }

        return std::string(ip_str);
    }
}
