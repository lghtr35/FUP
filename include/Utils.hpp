#pragma once

#include "Common.hpp"
#include "Message.hpp"
#include "Socket.hpp"
#include <zlib.h>

namespace fup {
    namespace util {
        bool IsResponseValid(const ConnectionId connId, const Message& response, Keyword expectedKeyword);

        std::vector<SequenceId> GetMissingPacketIds(const std::set<SequenceId>& receivedPackets, PacketCount packetCount);

        PacketCount CalculateTotalPacketCount(const FileSize& fileSize);

        long GetNextPossiblePacketId(const std::vector<bool>& packetsToSend);

        Checksum CalculateChecksum(const std::vector<Byte>& data);
        Checksum CalculateChecksum(const std::vector<Byte>& data, size_t start, size_t end);
        Checksum CalculateChecksum(const std::vector<Byte>& data, size_t start);

        bool ValidateChecksum(const std::vector<Byte>& data, Checksum checksum);

        SockAddr CreateUdpDestinationAddressFromTcp(const SockAddr& tcpAddr, Port udpPort);
        SockAddr CreateUdpDestinationAddressFromTcp(const std::shared_ptr<SockAddr> tcpAddr, Port udpPort);

        Port GetPortFromSockAddr(const SockAddr& addr);
        std::string GetIpFromSockAddr(const SockAddr& addr);
    }
}
