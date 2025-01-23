#include "Connection.hpp"

namespace fup
{
    Connection::Connection(Version v, int socketfd) {
        version = v;
        tcpSocketfd = socketfd;

        Message tcp_response = receiveMessage();
        if (tcp_response.header.keyword != ACK) {
            errno = EPROTO;
            std::ostringstream s;
            s << "server not accepting Connection. error: " << strerror(errno) << "\n ";
            throw std::runtime_error(s.str());
        }
        id = tcp_response.header.connectionId;

        isConnected = true;
    }

    Connection::~Connection() {
        disconnect();
        close(tcpSocketfd);
    }

    void Connection::disconnect() {
        if (isConnected) {
            Message msg;
            msg.header = { .body_size = 0, .connectionId = this->id, .version = this->version, .keyword = ABORT };
            this->sendMessage(msg);
        }
        close(tcpSocketfd);
    }

    bool Connection::getIsConnected() {
        return isConnected;
    }

    Message Connection::sendAndReceiveMessage(Message& request) {
        if (!isConnected) throw std::logic_error("Connection: invalid state \"communication_without_connection\"");
        sendMessage(request);
        return receiveMessage();
    }

    void Connection::sendMessage(Message& request)
    {
        if (!isConnected) throw std::logic_error("Connection: invalid state \"send_without_connection\"");
        int bytes_sent, Message_len;
        std::vector<uint8_t> buffer = request.serialize();
        Message_len = request.size();
        if (Message_len < 0)
        {
            throw std::runtime_error("Error in serializing Message.");
        }
        {
            std::unique_lock<std::mutex> lock(mutex);
            bytes_sent = send(tcpSocketfd, buffer.data(), Message_len, 0);
            validate_bytes_sent(bytes_sent, Message_len);
        }
    }

    Message Connection::receiveMessage() {
        Message response;
        int fixed_Message_size = response.header.size();
        int bytes_read = 0, bytes_parsed = 0;
        std::vector<uint8_t> fixed_Message_buffer(fixed_Message_size); // VLA
        {
            std::unique_lock<std::mutex> lock(mutex);
            bytes_read = recv(tcpSocketfd, fixed_Message_buffer.data(), fixed_Message_size, 0);
        }
        validate_bytes_read(bytes_read);
        validate_accumulated_bytes(bytes_read, fixed_Message_size, "header");

        bytes_parsed = response.header.deserialize(fixed_Message_buffer);
        if (bytes_parsed == -1 || bytes_parsed != bytes_read)
        {
            std::ostringstream s;
            s << "can not parse Message header. error: " << strerror(errno) << "\n";
            throw std::runtime_error(s.str());
        }
        response.body.resize(response.header.body_size); // VLA
        std::vector<uint8_t> buffer(BUFSIZ);
        bytes_read = 0;
        int offset = 0;
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (bytes_read >= 0 && offset < response.header.body_size)
            {
                bytes_read = recv(tcpSocketfd, buffer.data(), BUFSIZ, 0);
                validate_bytes_read(bytes_read);
                memcpy(response.body.data() + offset, buffer.data(), bytes_read);
                offset += bytes_read;
            }
        }
        validate_accumulated_bytes(offset, response.header.body_size, "body");

        return response;
    }

    Message Connection::server_accept() {
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        tcpSocketfd = accept(tcpSocketfd, (sockaddr*)&address, &addrlen);
        if (tcpSocketfd < 0) {
            std::ostringstream s;
            s << "socket fd error:  " << strerror(errno) << "\n ";
            throw std::runtime_error(s.str());
        }
        isConnected = true;
        int inet_addrlen = address.sin_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
        std::vector<char> url_chars(inet_addrlen);
        inet_ntop(address.sin_family, &address.sin_addr, url_chars.data(), inet_addrlen);

        return receiveMessage();
    }

    void validate_accumulated_bytes(int offset, int expected, std::string component) {
        if (offset != expected)
        {
            errno = EPROTO;
            std::ostringstream s;
            s << "server did not send Message " << component << " completely. error: " << strerror(errno) << "\n";
            throw std::runtime_error(s.str());
        }
    }

    void validate_bytes_read(int bytes_read) {
        if (bytes_read == 0)
        {
            errno = ECONNRESET;
            std::ostringstream s;
            s << "server closed Connection. error: " << strerror(errno) << "\n";
            throw std::runtime_error(s.str());
        }
        else if (bytes_read == -1)
        {
            errno = EPROTO;
            std::ostringstream s;
            s << "recv error: " << strerror(errno) << "\n";
            throw std::runtime_error(s.str());
        }
    }

    void validate_bytes_sent(int bytes_sent, int Message_len) {
        if (bytes_sent == -1)
        {
            std::ostringstream s;
            s << "error: " << strerror(errno) << "\n ";
            throw std::runtime_error(s.str());
        }
        else if (bytes_sent != Message_len)
        {
            errno = EPROTO;
            std::ostringstream s;
            s << "sent partial Message with" << bytes_sent << " bytes sent. error: " << strerror(errno) << "\n";
            throw std::runtime_error(s.str());
        }
    }
}