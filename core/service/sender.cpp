#pragma once
#include "sender.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            sender::sender(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<fup::core::interface::checksum> checksum)
            {
                tcp_socket = tcp;
                udp_socket = udp;
                checksum_service = checksum;
            }

            sender::~sender()
            {
                delete tcp_socket;
                delete udp_socket;
            }

            std::vector<char> *sender::create_checksum(std::vector<char> &data)
            {
                return checksum_service->create_checksum(data);
            }

            // Function to send data over TCP socket with error handling
            template <typename T>
            int sender::send_tcp(const T &payload)
            {
                std::vector<char> bytes = helper::serializer::serialize_payload(payload);
                size_t byteCount = bytes.size();

                size_t bytesSent = 0;
                try
                {
                    while (bytesSent < byteCount)
                    {
                        // Send data in chunks until all bytes are sent
                        bytesSent += tcp_socket->send(boost::asio::buffer(bytes.data() + bytesSent, byteCount - bytesSent));
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
                // Convert payload to byte vector
                std::vector<char> bytes = helper::serializer::serialize_payload(payload);
                size_t byteCount = bytes.size();

                size_t bytesSent = 0;
                try
                {
                    while (bytesSent < byteCount)
                    {
                        // Send data in chunks until all bytes are sent
                        bytesSent += udp_socket->send(boost::asio::buffer(bytes.data() + bytesSent, byteCount - bytesSent));
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
}
