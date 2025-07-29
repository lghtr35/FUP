#include "Client.hpp"

namespace fup {
    namespace client
    {
        Client::Client(unsigned int v, std::string sl, u_int16_t threadLimit, Port port) :
            version((fup::Version)v),
            saveLocation(sl),
            udpPort(port),
            threadLimit(threadLimit)
        {
        }

        Client::~Client() {}

        int Client::Upload(std::string destinationUrl, std::string destinationPort, std::string filename, std::string fileFullpath)
        {
            try {
                if (filename.size() > FUP_MAX_FILENAME_SIZE) {
                    ENODATA;
                    std::ostringstream s;
                    s << "filename is too long. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }

                std::shared_ptr<TcpWrapper> conn = std::make_shared<TcpWrapper>(this->version, destinationUrl, destinationPort);
                std::shared_ptr<std::ifstream> fileStream = std::make_shared<std::ifstream>(fileFullpath, std::ios::binary | std::ios::ate);
                if (!fileStream || !(*fileStream)) {
                    ENODATA;
                    std::ostringstream s;
                    s << "can not open file. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }
                FileSize fileSize = std::filesystem::file_size(fileFullpath);
                (*fileStream).seekg(0, std::ios::beg);
                std::shared_ptr<std::mutex> fileMutex = std::make_shared<std::mutex>();

                PacketCount packetCount = util::CalculateTotalPacketCount(fileSize);
                if (packetCount == 0) {
                    ENODATA;
                    std::ostringstream s;
                    s << "file size is zero. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }

                Message request(Header(this->version, conn->GetId(), UPLOAD), std::make_shared<fup::Upload>(this->udpPort, fileSize, packetCount, filename.size(), filename));
                Message response = conn->SendAndReceive(request);
                if (!util::IsResponseValid(conn->GetId(), response, DOWNLOAD))
                {
                    if (response.header.keyword == ABORT) {
                        std::ostringstream s;
                        s << "server not accepting upload request. error: " << strerror(errno) << "\n";
                        throw std::runtime_error(s.str());
                    }
                    EPROTO;
                    std::ostringstream s;
                    s << "unexpected response from server. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }

                auto downloadPtr = response.getBody<fup::Download>();
                if (!downloadPtr) {
                    throw std::runtime_error("Failed to cast response body to fup::Download");
                }

                std::shared_ptr<SockAddr> udpDestAddr = std::make_shared<SockAddr>(util::CreateUdpDestinationAddressFromTcp(*(conn->GetDestinationAddress()), downloadPtr->udpPort));
                std::shared_ptr<UdpWrapper> udpConn = std::make_shared<UdpWrapper>(version);

                std::shared_ptr<std::vector<bool>> packetsToSend = std::make_shared<std::vector<bool>>(packetCount, false);
                std::shared_ptr<std::mutex> packetsToSendMutex = std::make_shared<std::mutex>();
                std::unique_ptr<Threadpool> threadpool = std::make_unique<Threadpool>(threadLimit);
                std::shared_ptr<fup::StopSignal> stopSignal = std::make_shared<fup::StopSignal>();

                // Enqueue the resend thread to handle packet retransmission
                threadpool->Enqueue([
                    conn,
                    packetsToSendMutex,
                    packetsToSend,
                    stopSignal]() {
                        ListenResend(
                            conn,
                            packetsToSendMutex,
                            packetsToSend,
                            stopSignal,
                            1500
                        );
                    });

                // Enqueue remaining threads to send packets
                while (threadpool->GetWorkingThreadsCount() <= threadLimit) {
                    threadpool->Enqueue([
                        this,
                        conn,
                        udpConn,
                        udpDestAddr,
                        packetsToSendMutex,
                        fileMutex,
                        fileStream,
                        packetsToSend,
                        stopSignal]() {
                            TransmitPackets(
                                this->version,
                                conn,
                                udpConn,
                                udpDestAddr,
                                packetsToSendMutex,
                                fileMutex,
                                fileStream,
                                packetsToSend,
                                stopSignal
                            );
                        });

                    {
                        std::lock_guard<std::mutex> lock(stopSignal->mutex);
                        if (stopSignal->stop) {
                            break;
                        }
                    }
                }

                threadpool->WaitAll();
                (*fileStream).close();
                conn->Send(Message(Header(this->version, conn->GetId(), ACK), nullptr));

                return 0;
            }
            catch (std::exception& e) {
                std::cerr << e.what() << "\n";
                throw e;
            }
        }

        int Client::Download(std::string destUrl, std::string destPort, std::string filename, std::string downloadLocation)
        {
            try {
                std::shared_ptr<TcpWrapper> conn = std::make_shared<TcpWrapper>(this->version, destUrl, destPort);
                std::shared_ptr<UdpWrapper> udpconn = std::make_shared<UdpWrapper>(this->version, FUP_DEFAULT_UDP_PORT);

                Message request(Header(this->version, conn->GetId(), DOWNLOAD), std::make_shared<fup::Download>(udpconn->GetLocalAddress()->GetPort(), filename));

                Message response = conn->SendAndReceive(request);
                if (!util::IsResponseValid(conn->GetId(), response, UPLOAD))
                {
                    if (response.header.keyword == ABORT) {
                        std::ostringstream s;
                        s << "server not accepting download request. error: " << strerror(errno) << "\n";
                        throw std::runtime_error(s.str());
                    }
                    EPROTO;
                    std::ostringstream s;
                    s << "unexpected response from server. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }
                // Get the file information from the response
                std::shared_ptr<fup::Upload> uploadPtr = std::static_pointer_cast<fup::Upload>(response.body);
                if (!uploadPtr) {
                    throw std::runtime_error("Failed to cast response body to fup::Upload");
                }

                PacketCount packetCount = uploadPtr->packetCount;

                std::filesystem::path dir(downloadLocation);
                std::filesystem::path file(filename);
                std::shared_ptr<std::ofstream> fileStream = std::make_shared<std::ofstream>(dir / file, std::ios::binary | std::ios::trunc);
                if (!fileStream || !(*fileStream)) {
                    ENODATA;
                    std::ostringstream s;
                    s << "can not open file. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }
                std::shared_ptr<std::mutex> fileMutex = std::make_shared<std::mutex>();

                std::shared_ptr<std::vector<bool>> packetsReceived = std::make_shared<std::vector<bool>>(packetCount, false);

                std::shared_ptr<std::mutex> receivedPacketsMutex = std::make_shared<std::mutex>();
                std::unique_ptr<Threadpool> threadpool = std::make_unique<Threadpool>(threadLimit);
                std::shared_ptr<StopSignal> stopSignal = std::make_shared<StopSignal>();

                fd_set readSet;
                FD_ZERO(&readSet);
                FD_SET(udpconn->GetSocket()->Get(), &readSet);

                timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = 1500 * 1000;
                udpconn->GetSocket()->SetTimeout(1500);

                std::shared_ptr<fd_set> readSetPtr = std::make_shared<fd_set>(readSet);
                std::shared_ptr<timeval> timeoutPtr = std::make_shared<timeval>(timeout);

                threadpool->Enqueue([
                    this,
                    conn,
                    udpconn,
                    fileMutex,
                    fileStream,
                    packetsReceived,
                    receivedPacketsMutex,
                    stopSignal,
                    packetCount,
                    readSetPtr,
                    timeoutPtr
                ](){
                    ReceivePackets(
                        this->version,
                        conn,
                        udpconn,
                        fileMutex,
                        fileStream,
                        packetsReceived,
                        receivedPacketsMutex,
                        stopSignal,
                        packetCount,
                        readSetPtr,
                        timeoutPtr
                    );
                });


                while (true) {
                    Message msg = conn->Receive();

                    if (msg.header.keyword == ABORT) {
                        std::lock_guard<std::mutex> lock(stopSignal->mutex);
                        stopSignal->stop = true;
                        throw std::runtime_error("Remote server aborted the download.");
                    }
                    else {
                        std::cerr << "Unexpected message received. Keyword: " << msg.header.keyword << "\n";
                        continue;
                    }

                    {
                        std::lock_guard<std::mutex> lock(stopSignal->mutex);
                        if (stopSignal->stop) {
                            break;
                        }
                    }
                }

                threadpool->WaitAll();
                (*fileStream).close();
                conn->Send(Message(Header(this->version, conn->GetId(), ACK), nullptr));

                return 0;
            }
            catch (std::exception& e) {
                std::cerr << e.what() << "\n";
                throw e;
            }
        }

        int Client::ListFiles(std::string destinationUrl, std::string destinationPort, std::vector<std::string>& res) {
            try {
                std::shared_ptr<TcpWrapper> conn = std::make_shared<TcpWrapper>(this->version, destinationUrl, destinationPort);
                Message request(Header(this->version, conn->GetId(), LIST_FILES), nullptr);

                Message response = conn->SendAndReceive(request);
                if (!util::IsResponseValid(conn->GetId(), response, FILES)) {
                    if (response.header.keyword == ABORT) {
                        std::ostringstream s;
                        s << "server not accepting download request. error: " << strerror(errno) << "\n";
                        throw std::runtime_error(s.str());
                    }
                    EPROTO;
                    std::ostringstream s;
                    s << "unexpected response from server. error: " << strerror(errno) << "\n";
                    throw std::runtime_error(s.str());
                }

                std::shared_ptr<fup::Files> filesPtr = std::static_pointer_cast<fup::Files>(response.body);
                if (!filesPtr) {
                    throw std::runtime_error("Failed to cast response body to fup::Files");
                }

                res.resize(filesPtr->elemCount);
                for (size_t i = 0; i < filesPtr->elemCount; ++i) {
                    res[i] = filesPtr->filenames[i];
                }
            }
            catch (std::exception& e) {
                std::cerr << e.what() << "\n";
                throw e;
            }
            return 0;
        }
    } // namespace client
}
