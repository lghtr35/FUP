#include "Client.hpp"

namespace fup {
    namespace client
    {
        void ListenResend(std::shared_ptr<TcpWrapper> conn,
            std::shared_ptr<std::mutex> packetsToSendMutex,
            std::shared_ptr<std::vector<bool>> packetsToSend,
            std::shared_ptr<StopSignal> stopSignal,
            int timeoutMs) {
            std::shared_ptr<Socket> handle = conn->GetSocket();
            handle->SetBlocking(false);
            handle->SetTimeout(timeoutMs);

            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(handle->Get(), &readSet);
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = timeoutMs * 1000;

            while (true) {
                fd_set tempSet = readSet;
                if (select(handle->Get() + 1, &tempSet, nullptr, nullptr, &timeout) < 0) {
                    throw std::runtime_error("Error in select: " + std::string(strerror(errno)) + "\n");
                }
                if (FD_ISSET(handle->Get(), &tempSet)) {
                    Message message = conn->Receive();
                    if (message.header.keyword == RESEND) {
                        auto resend = message.getBody<fup::Resend>();
                        if (!resend) {
                            throw std::runtime_error("Failed to cast response body to fup::Resend");
                        }
                        std::lock_guard<std::mutex> lock(*packetsToSendMutex);
                        (*packetsToSend)[resend->seqId] = false;
                    }
                    else if (message.header.keyword == ACK) {
                        std::lock_guard<std::mutex> lock(stopSignal->mutex);
                        stopSignal->stop = true;
                        std::cout << "Received ACK from server. Stopping Resend listener...\n";
                        return;
                    }
                    else if (message.header.keyword == ABORT) {
                        std::cerr << "Server aborted the connection.\n";
                        return;
                    }
                    else {
                        std::cerr << "Unexpected message received: " << message.header.keyword << "\n";
                    }
                }
                {
                    std::lock_guard<std::mutex> lock(stopSignal->mutex);
                    if (stopSignal->stop) {
                        std::cout << "Stopping Resend listener...\n";
                        return;
                    }
                }
            }
        }

        void TransmitPackets(Version v,
            std::shared_ptr<TcpWrapper> conn,
            std::shared_ptr<UdpWrapper> udpconn,
            std::shared_ptr<SockAddr> udpDestAddr,
            std::shared_ptr<std::mutex> packetsToSendMutex,
            std::shared_ptr<std::mutex> fileMutex,
            std::shared_ptr<std::ifstream> fileStream,
            std::shared_ptr<std::vector<bool>> packetsToSend,
            std::shared_ptr<StopSignal> stopSignal) {
            long nextSeqId;
            while (true) {
                {
                    std::lock_guard<std::mutex> lock(stopSignal->mutex);
                    if (stopSignal->stop) {
                        std::cout << "Stopping TransmitPackets...\n";
                        return;
                    }
                }
                {
                    std::lock_guard<std::mutex> lock(*packetsToSendMutex);
                    nextSeqId = util::GetNextPossiblePacketId(*packetsToSend);
                    if (nextSeqId == -1) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        std::cout << "No packets to send. Waiting for stop signal or new packet to send...\n";
                        continue;
                    }
                }
                std::vector<Byte> buffer(FUP_PACKET_PAYLOAD_SIZE);
                {
                    std::lock_guard<std::mutex> lock(*fileMutex);
                    (*fileStream).seekg(nextSeqId * FUP_PACKET_PAYLOAD_SIZE, std::ios::beg);
                    (*fileStream).read(buffer.data(), FUP_PACKET_PAYLOAD_SIZE);
                    std::streamsize bytesRead = (*fileStream).gcount();
                    if (bytesRead == 0) {
                        std::cout << "No more data to read from file. Stopping TransmitPackets...\n";
                        break;
                    }
                }
                Checksum checksum = util::CalculateChecksum(buffer);
                Message packet(Header(v, conn->GetId(), PACKET), std::make_shared<fup::Packet>(nextSeqId, checksum, buffer));
                udpconn->Send(packet, udpDestAddr);
                {
                    std::lock_guard<std::mutex> lock(*packetsToSendMutex);
                    (*packetsToSend)[nextSeqId] = true;
                }
            }
        }

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
            std::shared_ptr<timeval> timeout) {
            while (true) {
                fd_set tempSet = *readSet;
                timeval tempTimeval = *timeout;
                if (select(udpconn->GetSocket()->Get() + 1, &tempSet, nullptr, nullptr, &tempTimeval) < 0) {
                    throw std::runtime_error("Error in select: " + std::string(strerror(errno)) + "\n");
                }
                if (FD_ISSET(udpconn->GetSocket()->Get(), &tempSet)) {
                    Message message = udpconn->Receive();
                    if (message.header.connectionId != conn->GetId()) {
                        std::cerr << "Received message with invalid connection ID: " << message.header.connectionId << "\n";
                        continue;
                    }
                    if (message.header.keyword == PACKET) {
                        auto packet = message.getBody<fup::Packet>();
                        if (!packet) {
                            // Do resend
                            RequestResend(v, conn, packet->seqId);
                            continue;
                        }
                        if (packet->seqId >= packetCount) {
                            std::cerr << "Received packet with invalid sequence ID: " << packet->seqId << "\n";
                            continue;
                        }
                        if (util::ValidateChecksum(packet->payload, packet->checksum)) {
                            {
                                std::lock_guard<std::mutex> lock(*fileMutex);
                                (*fileStream).seekp(packet->seqId * FUP_PACKET_PAYLOAD_SIZE, std::ios::beg);
                                (*fileStream).write(packet->payload.data(), packet->payload.size());
                                (*fileStream).flush();
                            }
                            std::cout << "Received and wrote packet with sequence ID: " << packet->seqId << "\n";
                            {
                                std::lock_guard<std::mutex> lock(*receivedPacketsMutex);
                                (*packetsReceived)[packet->seqId] = true;
                            }
                        }
                        else {
                            std::cerr << "Checksum validation failed for packet with sequence ID: " << packet->seqId << "\n";
                            // Request resend
                            RequestResend(v, conn, packet->seqId);
                        }
                    }
                    else {
                        std::cerr << "Unexpected message received: " << message.header.keyword << "\n";
                    }
                } // FD_ISSET

                {
                    std::lock_guard<std::mutex> lock(stopSignal->mutex);
                    if (stopSignal->stop) {
                        break;
                    }
                }
            } // main loop
        }

        void RequestResend(Version v, std::shared_ptr<TcpWrapper> conn, SequenceId seqId) {
            Message resendMessage(Header(v, conn->GetId(), RESEND), std::make_shared<fup::Resend>(seqId));
            conn->Send(resendMessage);
        }
    } // namespace client

}