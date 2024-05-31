
#include "receiver.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            receiver::receiver(int &tcp, int &udp)
            {
                tcp_socket = tcp;
                udp_socket = udp;
            }

            receiver::~receiver() {}

            std::string receiver::receive_message_identifier()
            {
                std::vector<char> received_data(2);

                size_t bytes_received = recv(tcp_socket, received_data.data(), 2, 0);
                if (bytes_received != 2)
                    throw new std::runtime_error("Read error in message id");

                return std::string(received_data.begin(), received_data.end());
            }

            fup::core::entity::packet receiver::receive_packet()
            {
                // Create a packet object to store received data
                fup::core::entity::packet received_package;

                // Reserve space for receiving packet data using already obtained packet size
                std::vector<char> received_data(metadata.file_packet_size + PACKET_FIXED_BUFFER_SIZE);

                // Receive packet data from the UDP socket
                size_t bytes_received = recv(udp_socket, received_data.data(), metadata.file_packet_size + PACKET_FIXED_BUFFER_SIZE, 0);

                if (PACKET_FIXED_BUFFER_SIZE + metadata.file_packet_size != bytes_received)
                {
                    throw std::runtime_error("Error receiving packet: Not enough data has arrived");
                }

                // Deserialize the received data into the packet object
                received_package.deserialize(received_data);

                // Return the received packet
                return received_package;
            }

            std::pair<unsigned int, unsigned int> receiver::receive_resend()
            {
                std::vector<char> buffer(GENERAL_BUFFER_SIZE);
                // Receive data from the socket
                size_t bytes_received = recv(tcp_socket, buffer.data(), GENERAL_BUFFER_SIZE, 0);

                // Check for errors
                if (bytes_received < 3)
                {
                    throw std::runtime_error("Error receiving response: Not enough data has arrived");
                }

                // Assuming the received data is a resend request in the format "<number>-<number>"
                std::string data(buffer.data(), bytes_received);

                std::pair<int, int> connection_id_seq_num_pair;
                int index = data.find("-");
                if (index != -1)
                {
                    // Extreact the connection number from firts part
                    int connection_number = std::stoi(data.substr(0, index));
                    // Extract the sequence number from the rest of the string
                    int sequence_number = std::stoi(data.substr(index + 1));
                    // Return the extracted identifiers
                    connection_id_seq_num_pair.first = connection_number;
                    connection_id_seq_num_pair.second = sequence_number;
                    return connection_id_seq_num_pair;
                }
                else
                {
                    throw std::runtime_error("Error receiving resend: Invalid resend request format");
                }
            }

            entity::metadata receiver::receive_metadata()
            {
                std::vector<char> buffer(METADATA_FIXED_BUFFER_SIZE + GENERAL_BUFFER_SIZE);

                // Receive data from the socket
                size_t bytes_received = recv(tcp_socket, buffer.data(), METADATA_FIXED_BUFFER_SIZE + GENERAL_BUFFER_SIZE, 0);

                // Check for errors

                if (METADATA_FIXED_BUFFER_SIZE >= bytes_received)
                {
                    throw std::runtime_error("Error receiving metadata: Not enough data has arrived");
                }

                entity::metadata received_metadata;
                received_metadata.deserialize(buffer);
                metadata = received_metadata;
                // Return the received metadata object
                return received_metadata;
            }
            // TODO: Think of a way to receive variable sized payload
            std::string receiver::receive_key()
            {
                // Buffer to store received data
                std::vector<char> buffer(GENERAL_BUFFER_SIZE);

                // Receive data from the TCP socket into the buffer
                size_t bytes_received = recv(tcp_socket, buffer.data(), GENERAL_BUFFER_SIZE, 0);

                // Convert received data to string
                std::string receivedKey(buffer.data(), bytes_received);

                return receivedKey;
            }

            entity::response receiver::receive_response()
            {
                // Buffer to store received data
                std::vector<char> buffer(RESPONSE_FIXED_BUFFER_SIZE);

                // Receive data from the TCP socket into the buffer
                size_t bytes_received = recv(tcp_socket, buffer.data(), RESPONSE_FIXED_BUFFER_SIZE, 0);

                // Check for errors
                if (RESPONSE_FIXED_BUFFER_SIZE != bytes_received)
                {
                    throw std::runtime_error("Error receiving response: Not enough data has arrived");
                }

                // Convert received data to string
                entity::response response;
                response.deserialize(buffer);

                return response;
            }

            entity::request receiver::receive_request()
            {
                // Buffer to store received data
                std::vector<char> buffer(REQUEST_FIXED_BUFFER_SIZE);

                // Receive data from the TCP socket into the buffer
                size_t bytes_received = recv(tcp_socket, buffer.data(), REQUEST_FIXED_BUFFER_SIZE, 0);

                // Check for errors
                if (REQUEST_FIXED_BUFFER_SIZE != bytes_received)
                {
                    throw std::runtime_error("Error receiving request: Not enough data has arrived");
                }

                // Convert received data to string
                entity::request response;
                response.deserialize(buffer);

                return response;
            }
        }
    }
}
