#include "Utils.hpp"

namespace fup {
    bool util::IsResponseValid(const ConnectionId connId, const Message& response, Keyword expectedKeyword) {
        if (response.header.keyword != expectedKeyword || connId != response.header.connectionId || response.header.bodySize == 0 || response.body == nullptr) {
            return false;
        }
        return true;
    }

    std::vector<SequenceId> util::GetMissingPacketIds(const std::set<SequenceId>& receivedPackets, PacketCount packetCount) {
        std::vector<SequenceId> missingPackets;
        for (SequenceId i = 0; i < packetCount; ++i) {
            if (receivedPackets.find(i) == receivedPackets.end()) {
                missingPackets.push_back(i);
            }
        }
        return missingPackets;
    }

    PacketCount util::CalculateTotalPacketCount(const FileSize& fileSize) {
        if (fileSize == 0) {
            throw std::invalid_argument("File size cannot be zero.");
        }
        PacketCount p = static_cast<PacketCount>((fileSize + FUP_PACKET_PAYLOAD_SIZE - 1) / FUP_PACKET_PAYLOAD_SIZE);

        return p;
    }

    long util::GetNextPossiblePacketId(const std::vector<bool>& packetsToSend) {
        long firstPacketToSend = -1;
        for (size_t i = 0; i < packetsToSend.size(); ++i) {
            if (!packetsToSend[i]) {
                firstPacketToSend = i;
                break;
            }
        }

        return firstPacketToSend;
    }

    Checksum util::CalculateChecksum(const std::vector<Byte>& data, size_t start, size_t end) {
        return crc32(0L, reinterpret_cast<const Bytef*>(&data[start]), end - start);
    }
    Checksum util::CalculateChecksum(const std::vector<Byte>& data, size_t start) {
        return CalculateChecksum(data, start, data.size());
    }
    Checksum util::CalculateChecksum(const std::vector<Byte>& data) {
        return CalculateChecksum(data, 0, data.size());
    }

    bool util::ValidateChecksum(const std::vector<Byte>& data, Checksum checksum) {
        Checksum calculatedChecksum = util::CalculateChecksum(data);
        return calculatedChecksum == checksum;
    }

    SockAddr util::CreateUdpDestinationAddressFromTcp(const SockAddr& tcpAddr, Port udpPort) {
        auto sockaddr = tcpAddr.GetAddr();
        if (sockaddr->ss_family == AF_INET) {
            struct sockaddr_in* in = reinterpret_cast<struct sockaddr_in*>(sockaddr);
            in->sin_port = htons(udpPort);
        }
        else if (sockaddr->ss_family == AF_INET6) {
            struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(sockaddr);
            in6->sin6_port = htons(udpPort);
        }
        return SockAddr(sockaddr, sizeof(sockaddr));
    }

    SockAddr util::CreateUdpDestinationAddressFromTcp(const std::shared_ptr<SockAddr> tcpAddr, Port udpPort) {
        auto sockaddr = tcpAddr->GetAddr();
        if (sockaddr->ss_family == AF_INET) {
            struct sockaddr_in* in = reinterpret_cast<struct sockaddr_in*>(sockaddr);
            in->sin_port = htons(udpPort);
        }
        else if (sockaddr->ss_family == AF_INET6) {
            struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(sockaddr);
            in6->sin6_port = htons(udpPort);
        }
        return SockAddr(sockaddr, sizeof(sockaddr));
    }

    Port util::GetPortFromSockAddr(const SockAddr& addr) {
        auto sockaddr = addr.GetAddr();
        if (sockaddr->ss_family == AF_INET) {
            return ntohs(((struct sockaddr_in*)sockaddr)->sin_port);
        }
        else if (sockaddr->ss_family == AF_INET6) {
            return ntohs(((struct sockaddr_in6*)sockaddr)->sin6_port);
        }
        else {
            throw std::runtime_error("Invalid address family");
        }
    }

    std::string util::GetIpFromSockAddr(const SockAddr& addr) {
        auto sockaddr = addr.GetAddr();
        char ipStr[INET6_ADDRSTRLEN];
        if (sockaddr->ss_family == AF_INET) {
            struct sockaddr_in* addr_in = (struct sockaddr_in*)sockaddr;
            inet_ntop(AF_INET, &addr_in->sin_addr, ipStr, sizeof(ipStr));
        }
        else if (sockaddr->ss_family == AF_INET6) {
            struct sockaddr_in6* addr_in6 = (struct sockaddr_in6*)sockaddr;
            inet_ntop(AF_INET6, &addr_in6->sin6_addr, ipStr, sizeof(ipStr));
        }
        else {
            throw std::runtime_error("Invalid address family");
        }
        return std::string(ipStr);
    }
}