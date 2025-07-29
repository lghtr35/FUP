#include "Client.hpp"

namespace fup
{
    namespace client
    {
        void TcpWrapper::Send(const Message& msg) {
            Message cpy = msg;
            try
            {
                cpy.header.connectionId = id;
                std::vector<Byte> buffer = cpy.serialize();
                int bytesSent = socket->Send(buffer.data(), buffer.size());
                if (bytesSent == 0 || static_cast<int>(buffer.size()) != bytesSent) {
                    throw std::runtime_error("Bytes sent is not equal to the expected amount.");
                }
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error("TcpWrapper::Send: Failed to send the complete message: " + std::string(e.what()));
            }

        }

        Message TcpWrapper::Receive() {
            MessageSize headerSize = Header::size();
            std::vector<Byte> headerBuffer(headerSize);
            MessageSize totalBytesRead = 0;

            try
            {
                while (totalBytesRead < headerSize) {
                    int bytesRead = socket->Receive(headerBuffer.data() + totalBytesRead, headerSize - totalBytesRead);
                    totalBytesRead += bytesRead;
                }
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error("TcpWrapper::Receive: Failed to receive message header: " + std::string(e.what()));
            }

            // 2. Deserialize the header to get the body size
            Header header(headerBuffer);
            std::vector<Byte> bodyBuffer(header.bodySize);

            // 3. Read the remaining body bytes
            MessageSize bytesToRead = header.bodySize;
            MessageSize currentBodyBytesRead = 0;

            try {
                while (currentBodyBytesRead < bytesToRead) {
                    int bytesRead = socket->Receive(bodyBuffer.data() + headerSize + currentBodyBytesRead, bytesToRead - currentBodyBytesRead);
                    currentBodyBytesRead += bytesRead;
                }
            }
            catch (const std::exception& e) {
                throw std::runtime_error("TcpWrapper::Receive: Failed to receive message body: " + std::string(e.what()));
            }

            // 4. Deserialize the complete message
            fup::Message receivedMessage(header, bodyBuffer);

            if (id == 0) {
                id = receivedMessage.header.connectionId;
            }

            return receivedMessage;
        }


        Message TcpWrapper::SendAndReceive(const Message& message) {
            try
            {
                Send(message);
                auto res = Receive();

                return res;
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error("TcpWrapper::SendAndReceive: An error occured: " + std::string(e.what()));
            }
        }

        TcpWrapper::TcpWrapper(Version v, std::string destinationUrl, std::string destinationPort) {
            version = v;
            this->socket = std::make_shared<Socket>(true);
            this->socket->Connect(destinationUrl, destinationPort);
        }

        std::shared_ptr<SockAddr> TcpWrapper::GetLocalAddress() const {
            return socket->GetLocalAddr();
        }

        std::shared_ptr<SockAddr> TcpWrapper::GetDestinationAddress() const {
            return socket->GetDestinationAddr();
        }
    } // namespace client

} // namespace fup
