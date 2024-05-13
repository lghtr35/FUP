#pragma once

#include <core/entity/request.hpp>
#include <core/helper/helper.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> request::serialize() const
            {
                // Convert is_download to big-endian and copy
                std::vector<char> is_download_bytes(sizeof(bool));
                std::memcpy(is_download_bytes.data(), &is_download, sizeof(bool));

                std::vector<char> udp_port_bytes(sizeof(unsigned int));
                unsigned int udp_port_be = htonl(udp_port);
                std::memcpy(udp_port_bytes.data(), &udp_port_be, sizeof(unsigned int));

                std::vector<char> connection_id_bytes(sizeof(unsigned int));
                unsigned int connection_id_be = htonl(connection_id);
                std::memcpy(connection_id_bytes.data(), &connection_id_be, sizeof(unsigned int));

                // Convert is_download to big-endian and copy
                std::vector<char> package_size_bytes(sizeof(unsigned int));
                unsigned int package_size_be = htonl(package_size);
                std::memcpy(package_size_bytes.data(), &package_size_be, sizeof(unsigned int));

                // Convert file_name length to big-endian and copy
                uint32_t file_name_len_be = htonl(file_name.size());
                std::vector<char> file_name_len_bytes(sizeof(uint32_t));
                std::memcpy(file_name_len_bytes.data(), &file_name_len_be, sizeof(uint32_t));

                // Add message prefix for identifying requests than resends
                std::string message_prefix("SE");
                std::vector<char> message_identifier(message_prefix.begin(), message_prefix.end());

                return helper::serializer::concatenate_vectors<char>({message_identifier,
                                                                      is_download_bytes,
                                                                      udp_port_bytes,
                                                                      connection_id_bytes,
                                                                      package_size_bytes,
                                                                      file_name_len_bytes,
                                                                      std::vector<char>(file_name.begin(), file_name.end())});
            }

            size_t request::deserialize(const std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize is_download
                std::memcpy(&is_download, data.data() + offset, sizeof(bool));
                offset += sizeof(bool);

                // Deserialize udp_port
                std::memcpy(&udp_port, data.data() + offset, sizeof(unsigned int));
                udp_port = ntohl(udp_port);
                offset += sizeof(unsigned int);

                // Deserialize connection_id
                std::memcpy(&connection_id, data.data() + offset, sizeof(unsigned int));
                connection_id = ntohl(connection_id);
                offset += sizeof(unsigned int);

                // Deserialize package_size
                std::memcpy(&package_size, data.data() + offset, sizeof(unsigned int));
                package_size = ntohl(package_size);
                offset += sizeof(unsigned int);

                // Deserialize file_name length
                uint32_t fileNameLen;
                std::memcpy(&fileNameLen, data.data() + offset, sizeof(uint32_t));
                fileNameLen = ntohl(fileNameLen);
                offset += sizeof(uint32_t);

                // Deserialize file_name
                file_name.resize(fileNameLen);
                std::memcpy(file_name.data(), data.data() + offset, fileNameLen);
                offset += fileNameLen;

                return offset;
            }
        }
    }
}