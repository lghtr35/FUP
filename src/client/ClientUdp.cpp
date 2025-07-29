#include "Client.hpp"

namespace fup
{
    namespace client
    {
        void UdpWrapper::Send(const Message& msg, const std::shared_ptr<SockAddr> dest) {
            try {
                std::vector<Byte> buffer = msg.serialize();
                int bytesSent = socket->Send(
                    buffer.data(),
                    buffer.size(),
                    dest
                );

                if (bytesSent == 0 || static_cast<int>(buffer.size()) != bytesSent) {
                    throw std::runtime_error("Bytes sent is not equal to the expected amount.");
                }
            }
            catch (const std::exception& e) {
                throw std::runtime_error("UdpWrapper::Send: Failed to send the complete message: " + std::string(e.what()));
            }
        }

        Message UdpWrapper::Receive() {
            std::vector<Byte> buffer(FUP_PACKET_MESSAGE_SIZE);

            int bytesReceived = socket->Receive(
                buffer.data(),
                buffer.size()
            );

            if (bytesReceived <= 0) {
                throw std::runtime_error("UdpWrapper::Receive: No data received.");
            }

            buffer.resize(bytesReceived);
            return Message(buffer);
        }

        UdpWrapper::UdpWrapper(Version v, fup::Port p) {
            version = v;
            socket = std::make_shared<Socket>(false, p);
        }

        std::shared_ptr<SockAddr> UdpWrapper::GetLocalAddress() const {
            return socket->GetLocalAddr();
        }

        std::shared_ptr<SockAddr> UdpWrapper::GetDestinationAddress() const {
            return socket->GetDestinationAddr();
        }
    } // namespace client

} // namespace fup
