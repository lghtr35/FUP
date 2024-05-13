#pragma once
#include "receiver.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            receiver::receiver(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp)
            {
                tcp_socket = tcp;
                udp_socket = udp;
            }

            receiver::~receiver()
            {
                delete metadata;
                delete tcp_socket;
                delete udp_socket;
            }

            std::string receiver::receive_message_identifier()
            {
                std::vector<char> received_data(2);

                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(received_data), 0, error);

                if (error)
                {
                    // Handle the error (e.g., log, throw an exception)
                    // For simplicity, let's just print the error message
                    throw std::runtime_error("Error receiving identifier: " + error.message());
                }

                return std::string(received_data.begin(), received_data.end());
            }

            fup::core::entity::package *receiver::receive_package()
            {
                // Create a package object to store received data
                fup::core::entity::package *received_package = new fup::core::entity::package();

                // Reserve space for receiving package data using already obtained package size
                std::vector<char> received_data(metadata->file_package_size + PACKAGE_FIXED_BUFFER_SIZE);

                // Receive package data from the UDP socket
                boost::system::error_code error;
                size_t bytes_received = udp_socket->receive(boost::asio::buffer(received_data), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle the error (e.g., log, throw an exception)
                    // For simplicity, let's just print the error message
                    throw std::runtime_error("Error receiving package: " + error.message());
                }

                if (PACKAGE_FIXED_BUFFER_SIZE + metadata->file_package_size != bytes_received)
                {
                    throw std::runtime_error("Error receiving package: Not enough data has arrived");
                }

                // Deserialize the received data into the package object
                received_package->deserialize(received_data);

                // Return the received package
                return received_package;
            }

            std::pair<int, int> receiver::receive_resend()
            {
                std::array<char, RESEND_BUFFER_SIZE> buffer;
                // Receive data from the socket
                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(buffer), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle error
                    throw std::runtime_error("Error receiving key: " + error.message());
                }

                if (RESEND_BUFFER_SIZE != bytes_received)
                {
                    throw std::runtime_error("Error receiving response: Not enough data has arrived");
                }

                // Assuming the received data is a resend request in the format "<number>-<number>"
                std::string data(buffer.data(), bytes_received);

                std::pair<int, int> connection_id_seq_num_pair;
                int index = data.find("-");
                if (index == -1)
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

            entity::metadata *receiver::receive_metadata()
            {
                std::vector<char> buffer(METADATA_FIXED_BUFFER_SIZE + 1024);

                // Receive data from the socket
                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(buffer), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle error
                    throw std::runtime_error("Error receiving metadata: " + error.message());
                }

                if (METADATA_FIXED_BUFFER_SIZE > bytes_received)
                {
                    throw std::runtime_error("Error receiving metadata: Not enough data has arrived");
                }

                entity::metadata *received_metadata = new entity::metadata();
                received_metadata->deserialize(buffer);
                metadata = received_metadata;
                // Return the received metadata object
                return received_metadata;
            }

            std::string receiver::receive_key()
            {
                // Buffer to store received data
                std::array<char, KEY_BUFFER_SIZE> buffer;

                // Receive data from the TCP socket into the buffer
                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(buffer), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle error
                    throw std::runtime_error("Error receiving key: " + error.message());
                }

                if (KEY_BUFFER_SIZE != bytes_received)
                {
                    throw std::runtime_error("Error receiving key: Not enough data has arrived");
                }

                // Convert received data to string
                std::string receivedKey(buffer.data(), bytes_received);

                return receivedKey;
            }

            entity::response *receiver::receive_response()
            {
                // Buffer to store received data
                std::vector<char> buffer(RESPONSE_BUFFER_SIZE);

                // Receive data from the TCP socket into the buffer
                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(buffer), 0, error);

                // Check for errors
                if (error)
                {
                    throw std::runtime_error("Error receiving response: " + error.message());
                }

                if (RESPONSE_BUFFER_SIZE != bytes_received)
                {
                    throw std::runtime_error("Error receiving response: Not enough data has arrived");
                }

                // Convert received data to string
                entity::response *response = new entity::response();
                response->deserialize(buffer);

                return response;
            }
        }
    }
}
