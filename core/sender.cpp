#pragma once

#include <core/sender.hpp>
#include <iostream>

namespace fup
{
    namespace core
    {
        // Serialization function for string payload
        std::vector<uint8_t> sender::serialize_payload(const std::string &p) const
        {
            return std::vector<uint8_t>(p.begin(), p.end());
        }

        // Serialization function for entities that are serializable
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<entity::serializable, T>>>
        std::vector<uint8_t> sender::serialize_payload(const T &p) const
        {
            return p.serialize();
        }

        // Function to send data over TCP socket with error handling
        template <typename T>
        int sender::send_tcp(boost::shared_ptr<boost::asio::ip::tcp::socket> &handler, const T &payload) const
        {
            std::vector<uint8_t> bytes = serialize_payload(payload);
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
        int sender::send_udp(boost::shared_ptr<boost::asio::ip::udp::socket> &handler, const T &payload) const
        {
            // Convert payload to byte vector
            std::vector<uint8_t> bytes = serialize_payload(payload);
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

        // Function to send a key over TCP socket
        int sender::send_key(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const std::string key) const
        {
            return send_tcp(handler, key);
        }

        // Function to send metadata over TCP socket
        int sender::send_metadata(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const entity::metadata &metadata) const
        {
            return send_tcp(handler, metadata);
        }

        // Function to send a packet over UDP socket
        int sender::send_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler, const entity::packet &packet) const
        {
            return send_udp(handler, packet);
        }

        // Function to send a resend request over TCP socket
        int sender::send_resend(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, int packageNumber) const
        {
            return send_tcp(handler, "RE" + std::to_string(packageNumber));
        }
    };
}
