#pragma once

#include "Common.hpp"
#include "Communications.hpp"
#include "Threadpool.hpp"
#include "Utils.hpp"

namespace fup
{
    namespace client {
        /*
        * Can download one file at a time
        * Might need refactoring to allow more versions of FUP to be implemented as submodules
        */
        class Client
        {
        public:
            Client(unsigned int version, std::string save_location, uint16_t thread_limit = FUP_DEFAULT_CLIENT_MAX_THREAD, Port port = FUP_DEFAULT_UDP_PORT);
            ~Client();

            int Upload(std::string destinationUrl, std::string destinationPort, std::string filename, std::string fileFullpath);
            int Download(std::string destinationUrl, std::string destinationPort, std::string filename, std::string downloadLocation);
            int ListFiles(std::string destinationUrl, std::string destinationPort, std::vector<std::string>& res);

        private:
            Version version;
            std::string saveLocation;
            Port udpPort;
            uint16_t maxThreadPoolSize;
        };

        void TransmitPackets(Version v,
            std::shared_ptr<TcpWrapper> conn,
            std::shared_ptr<UdpWrapper> udpconn,
            std::shared_ptr<SockAddr> udpDestAddr,
            std::shared_ptr<std::mutex> packetsToSendMutex,
            std::shared_ptr<std::mutex> fileMutex,
            std::shared_ptr<std::ifstream> fileStream,
            std::shared_ptr<std::vector<bool>> packetsToSend,
            std::shared_ptr<StopSignal> stopSignal);

        void ListenResend(std::shared_ptr<TcpWrapper> conn,
            std::shared_ptr<std::mutex> packetsToSendMutex,
            std::shared_ptr<std::vector<bool>> packetsToSend,
            std::shared_ptr<StopSignal> stopSignal,
            int timeoutMs = 1500);

        void ReceivePackets(Version v,
            std::shared_ptr<TcpWrapper> conn,
            std::shared_ptr<UdpWrapper> udpconn,
            std::shared_ptr<std::mutex> fileMutex,
            std::shared_ptr<std::ofstream> fileStream,
            std::shared_ptr<std::vector<bool>> packetsReceived,
            std::shared_ptr<std::mutex> receivedPacketsMutex,
            std::shared_ptr<StopSignal> stopSignal,
            PacketCount packetCount,
            std::shared_ptr<fd_set> readSet,
            std::shared_ptr<timeval> timeout);

        void RequestResend(Version v, std::shared_ptr<TcpWrapper> conn, SequenceId seqId);
    }
}

