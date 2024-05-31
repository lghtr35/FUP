
#include "sender.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            sender::sender(int tcp, int udp)
            {
                tcp_socket = tcp;
                udp_socket = udp;
            }

            sender::~sender() {}

            // Function to send data over TCP socket with error handling
            template <typename T>
            int sender::send_tcp(T &payload)
            {
                std::vector<char> bytes = entity::serializer::serialize_payload(payload);
                size_t byteCount = bytes.size();

                size_t bytesSent = send(tcp_socket, bytes.data(), byteCount, 0);
                if (bytesSent != byteCount)
                {
                    throw new std::runtime_error("TCP send error");
                }

                return bytesSent;
            }

            // Function to send data over UDP socket with error handling
            template <typename T>
            int sender::send_udp(T &payload)
            {
                // Convert payload to byte vector
                std::vector<char> bytes = entity::serializer::serialize_payload(payload);
                size_t byteCount = bytes.size();

                size_t bytesSent = send(udp_socket, bytes.data(), byteCount, 0);
                if (bytesSent != byteCount)
                {
                    throw new std::runtime_error("UDP send error");
                }

                return bytesSent;
            }
            // Function to send response to start udp data transfer
            int sender::send_response(entity::response &response)
            {
                return send_tcp(response);
            }

            // Function to send a key over TCP socket
            int sender::send_key(std::string key)
            {
                return send_tcp(key);
            }

            // Function to send metadata over TCP socket
            int sender::send_metadata(entity::metadata &metadata)
            {
                return send_tcp(metadata);
            }

            // Function to send a packet over UDP socket
            int sender::send_packet(entity::packet &packet)
            {
                return send_udp(packet);
            }

            // Function to send a resend request over TCP socket
            int sender::send_resend(unsigned int connection_id, unsigned int packet_number)
            {
                std::string resend_str = "RE" + std::to_string(connection_id) + "-" + std::to_string(packet_number);
                return send_tcp(resend_str);
            }

            int sender::send_request(fup::core::entity::request &request)
            {
                return send_tcp(request);
            }
        };
    }
}
