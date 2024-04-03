#pragma once
#include "receiver.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            receiver::receiver(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<interface::checksum> checksum)
            {
                tcp_socket = tcp;
                udp_socket = udp;
                checksum_service = checksum;
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
                    std::cerr << "Error receiving identifier: " << error.message() << std::endl;

                    return nullptr;
                }

                return std::string(received_data.begin(), received_data.end());
            }

            fup::core::entity::packet *receiver::receive_packet()
            {
                // Create a packet object to store received data
                fup::core::entity::packet *received_packet = new fup::core::entity::packet();

                // Reserve space for receiving packet data using already obtained package size
                std::vector<char> received_data(metadata->file_packet_size + PACKET_FIXED_BUFFER_SIZE);

                // Receive packet data from the UDP socket
                boost::system::error_code error;
                size_t bytes_received = udp_socket->receive(boost::asio::buffer(received_data), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle the error (e.g., log, throw an exception)
                    // For simplicity, let's just print the error message
                    std::cerr << "Error receiving packet: " << error.message() << std::endl;

                    // Clean up and return nullptr
                    delete received_packet;
                    return nullptr;
                }

                // Deserialize the received data into the packet object
                try
                {
                    received_packet->deserialize(received_data);
                }
                catch (const std::exception &e)
                {
                    // Handle deserialization error (e.g., log, throw an exception)
                    // For simplicity, let's just print the error message
                    std::cerr << "Error deserializing packet: " << e.what() << std::endl;

                    // Clean up and return nullptr
                    delete received_packet;
                    return nullptr;
                }

                // Return the received packet
                return received_packet;
            }

            std::pair<int, int> receiver::receive_resend()
            {
                try
                {
                    std::array<char, RESEND_BUFFER_SIZE> buffer;
                    // Receive data from the socket
                    size_t bytesReceived = tcp_socket->receive(boost::asio::buffer(buffer));
                    // Assuming the received data is a resend request in the format "RE<number>"
                    std::string data(buffer.data(), bytesReceived);

                    // 24-52
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
                        // If the received data does not start with "RE", log an error
                        std::cerr << "Invalid resend request format: " << data << std::endl;
                        // Return an appropriate error code or throw an exception
                        throw std::runtime_error("Invalid resend request format");
                    }
                }
                catch (const boost::system::system_error &e)
                {
                    std::cerr << "Receive resend error: " << e.what() << std::endl;
                    throw std::runtime_error("Unknown request arrived");
                }
            }

            entity::metadata *receiver::receive_metadata()
            {
                try
                {
                    std::vector<char> buffer(METADATA_BUFFER_SIZE);

                    // Receive data from the socket
                    size_t bytesReceived = tcp_socket->receive(boost::asio::buffer(buffer));

                    entity::metadata *received_metadata = new entity::metadata();
                    received_metadata->deserialize(buffer);
                    metadata = received_metadata;
                    // Return the received metadata object
                    return received_metadata;
                }
                catch (const boost::system::system_error &e)
                {
                    // Log the error using std::cerr
                    std::cerr << "Receive metadata error: " << e.what() << std::endl;

                    throw std::runtime_error("Unknown request arrived");
                }
            }

            std::string receiver::receive_key()
            {
                // Buffer to store received data
                std::array<char, KEY_BUFFER_SIZE> buffer;

                // Receive data from the TCP socket into the buffer
                boost::system::error_code error;
                size_t bytesReceived = tcp_socket->read_some(boost::asio::buffer(buffer), error);

                // Check for errors
                if (error)
                {
                    // Handle error
                    throw std::runtime_error("Error receiving key: " + error.message());
                }

                // Convert received data to string
                std::string receivedKey(buffer.data(), bytesReceived);

                return receivedKey;
            }

            bool receiver::receive_ok()
            {
                // Buffer to store received data
                std::array<char, OK_BUFFER_SIZE> buffer;

                // Receive data from the TCP socket into the buffer
                boost::system::error_code error;
                size_t bytes_received = tcp_socket->receive(boost::asio::buffer(buffer), 0, error);

                // Check for errors
                if (error)
                {
                    // Handle error
                    throw std::runtime_error("Error receiving key: " + error.message());
                }

                // Convert received data to string
                std::string ok_string(buffer.data(), bytes_received);

                if (ok_string == std::string("OK"))
                {
                    return true;
                }
                return false;
            }
        }
    }
}
