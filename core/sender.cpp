#pragma once
#include <core/sender.hpp>

namespace fup
{
    namespace core
    {
        sender::sender(int port)
        {
            hasher = new blake3_hasher();
            socket_factory = new helper::socket_singleton_factory();
        }

        sender::~sender()
        {
            delete hasher;
            delete socket_factory;
        }

        std::vector<uint8_t> *sender::create_checksum(std::vector<uint8_t> &data)
        {
            return helper::checksum::create_checksum(hasher, data);
        }

        // Function to send data over TCP socket with error handling
        template <typename T>
        int sender::send_tcp(const T &payload)
        {
            boost::asio::ip::tcp::socket *handler = socket_factory.get_tcp();
            std::vector<uint8_t> bytes = helper::serializer::serialize_payload(payload);
            size_t byteCount = bytes.size();

            size_t bytesSent = 0;
            try
            {
                while (bytesSent < byteCount)
                {
                    // Send data in chunks until all bytes are sent
                    bytesSent += handler->send(boost::asio::buffer(bytes.data() + bytesSent, byteCount - bytesSent));
                }
            }
            catch (const boost::system::system_error &e)
            {
                // Log the error using std::cerr
                std::cerr << "TCP socket send error: " << e.what() << std::endl;

                // For demonstration, returning -1 here, but handle error as appropriate in your application
                return -1;
            }

            return bytesSent;
        }

        // Function to send data over UDP socket with error handling
        template <typename T>
        int sender::send_udp(const T &payload)
        {
            boost::asio::ip::udp::socket *handler = socket_factory.get_tcp();
            // Convert payload to byte vector
            std::vector<uint8_t> bytes = helper::serializer::serialize_payload(payload);
            size_t byteCount = bytes.size();

            size_t bytesSent = 0;
            try
            {
                while (bytesSent < byteCount)
                {
                    // Send data in chunks until all bytes are sent
                    bytesSent += handler->send(boost::asio::buffer(bytes.data() + bytesSent, byteCount - bytesSent));
                }
            }
            catch (const boost::system::system_error &e)
            {
                // Log the error using std::cerr
                std::cerr << "UDP socket send error: " << e.what() << std::endl;

                // For demonstration, returning -1 here, but handle error as appropriate in your application
                return -1;
            }

            return bytesSent;
        }
        // Function to send ok to start udp data transfer
        int sender::send_ok()
        {
            return send_tcp(new std::string("OK"));
        }

        // Function to send a key over TCP socket
        int sender::send_key(const std::string key)
        {
            return send_tcp(key);
        }

        // Function to send metadata over TCP socket
        int sender::send_metadata(const entity::metadata &metadata)
        {
            return send_tcp(metadata);
        }

        // Function to send a packet over UDP socket
        int sender::send_packet(const entity::packet &packet)
        {
            return send_udp(packet);
        }

        // Function to send a resend request over TCP socket
        int sender::send_resend(const int &package_number)
        {
            std::string resend_str = "RE" + std::to_string(package_number);
            return send_tcp(resend_str);
        }

        int sender::send_request(const fup::core::entity::request &request)
        {
            return send_tcp(request);
        }
    };
}
