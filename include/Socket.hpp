#pragma once

#include "Common.hpp"

namespace fup
{
    class SockAddr {
    private:
        std::unique_ptr<Byte[]> addrBuf;
        socklen_t addrLen;
    public:
        Port GetPort();
        std::string GetIpAddress();
        struct sockaddr_storage* GetAddr() const { return reinterpret_cast<struct sockaddr_storage*>(addrBuf.get()); }
        socklen_t GetAddrLen() const { return addrLen; }
        void SetAddr(const sockaddr_storage* addr, socklen_t addrlen) {
            if (addr && addrlen > 0) {
                addrBuf = std::make_unique<Byte[]>(addrlen);
                std::memcpy(addrBuf.get(), addr, addrlen);
                addrLen = addrlen;
            }
            else {
                addrBuf.reset();
                addrLen = 0;
            }
        }

        SockAddr() : addrBuf(nullptr), addrLen(0) {}
        SockAddr(const sockaddr_storage* a, socklen_t l) : addrBuf(nullptr), addrLen(0) {
            SetAddr(a, l);
        }
        SockAddr(const SockAddr& other) : addrBuf(nullptr), addrLen(0) {
            SetAddr(other.GetAddr(), other.addrLen);
        }
        SockAddr& operator=(const SockAddr& other) {
            if (this != &other) {
                SetAddr(other.GetAddr(), other.addrLen);
            }
            return *this;
        }
        SockAddr(SockAddr&& other) noexcept : addrBuf(std::move(other.addrBuf)), addrLen(other.addrLen) {
            other.addrLen = 0;
        }
        SockAddr& operator=(SockAddr&& other) noexcept {
            if (this != &other) {
                addrBuf = std::move(other.addrBuf);
                addrLen = other.addrLen;
                other.addrLen = 0;
            }
            return *this;
        }
        ~SockAddr() = default;
    };

    class Socket {
    public:
        Socket(bool isTcp, Port port = 0);
        Socket(bool isTcp, SocketHandle sockfd, std::shared_ptr<SockAddr> local, std::shared_ptr<SockAddr> dest);
        ~Socket() {
            if (handle != INVALID_SOCKET_HANDLE)
                socket_close(handle);
        }

        SocketHandle Get() const {
            return handle;
        }
        int Send(const void* buffer, int length, std::shared_ptr<SockAddr> dest = nullptr);
        int Receive(void* buffer, size_t length);
        void Bind(const struct sockaddr* addr, socklen_t addrlen);
        void Connect(const std::string& url, const std::string& port);
        void Listen(int backlog);
        Socket Accept();
        void Close();

        bool IsBlocking() const;
        void SetBlocking(bool blocking);
        bool IsTcp() const;
        void SetTimeout(int timeoutMs);
        std::shared_ptr<SockAddr> GetLocalAddr();
        std::shared_ptr<SockAddr> GetDestinationAddr();
    private:
        SockAddr getDestinationSockAddr(const std::string& url, const std::string& port);
        int sendToBase(const void* buffer, int length);
        int sendBase(const void* buffer, int length);

        SocketHandle handle;
        std::shared_ptr<SockAddr> destAddr;
        std::shared_ptr<SockAddr> localAddr;
        std::mutex mutex;
        bool isBlocking = true;
        bool isTcp;
    };
} // namespace fup
