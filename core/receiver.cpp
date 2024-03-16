#pragma once
#include <core/receiver.hpp>

namespace fup
{
    namespace core
    {
        receiver::receiver()
        {
            hasher = new blake3_hasher();
            socket_factory = new helper::socket_singleton_factory();
        }

        receiver::~receiver()
        {
            delete hasher;
            delete socket_factory;
            delete metadata;
        }

        bool receiver::validate_checksum(std::vector<uint8_t> &data, std::vector<uint8_t> &checksum)
        {
            return helper::checksum::validate_checksum(hasher, data, checksum);
        }

        std::vector<uint8_t> *receiver::create_checksum(std::vector<uint8_t> &data)
        {
            return helper::checksum::create_checksum(hasher, data);
        }

        fup::core::entity::packet *receiver::receive_packet(unsigned short &fps)
        {
            boost::asio::ip::udp::socket *handler = socket_factory->get_udp();
            // Create a packet object to store received data
            fup::core::entity::packet *received_packet = new fup::core::entity::packet();

            // Reserve space for receiving packet data using already obtained package size
            std::vector<uint8_t> received_data(metadata->file_package_size + PACKET_FIXED_BUFFER_SIZE);

            // Receive packet data from the UDP socket
            boost::system::error_code error;
            size_t bytes_received = handler->receive(boost::asio::buffer(received_data), 0, error);

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

        int receiver::receive_resend()
        {
            boost::asio::ip::tcp::socket *handler = socket_factory->get_tcp();
            try
            {

                std::array<char, RESEND_BUFFER_SIZE> buffer;

                // Receive data from the socket
                size_t bytesReceived = handler->receive(boost::asio::buffer(buffer));

                // Assuming the received data is a resend request in the format "RE<number>"
                std::string data(buffer.data(), bytesReceived);

                // Check if the received data starts with "RE"
                if (data.substr(0, 2) == "RE")
                {
                    // Extract the sequence number from the rest of the string
                    int sequenceNumber = std::stoi(data.substr(2));
                    // Return the extracted sequence number
                    return sequenceNumber;
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
            boost::asio::ip::tcp::socket *handler = socket_factory->get_tcp();
            try
            {
                std::vector<uint8_t> buffer(METADATA_BUFFER_SIZE);

                // Receive data from the socket
                size_t bytesReceived = handler->receive(boost::asio::buffer(buffer));

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
            boost::asio::ip::tcp::socket *handler = socket_factory->get_tcp();
            // Buffer to store received data
            std::array<char, KEY_BUFFER_SIZE> buffer;

            // Receive data from the TCP socket into the buffer
            boost::system::error_code error;
            size_t bytesReceived = handler->read_some(boost::asio::buffer(buffer), error);

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
            boost::asio::ip::tcp::socket *handler = socket_factory->get_tcp();
            // Buffer to store received data
            std::array<char, OK_BUFFER_SIZE> buffer;

            // Receive data from the TCP socket into the buffer
            boost::system::error_code error;
            size_t bytes_received = handler->receive(boost::asio::buffer(buffer), 0, error);

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
