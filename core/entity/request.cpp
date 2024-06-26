
#include "request.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> request::serialize()
            {
                // Convert is_download to big-endian and copy
                std::vector<char> is_download_bytes(sizeof(bool));
                std::memcpy(is_download_bytes.data(), &is_download, sizeof(bool));

                std::vector<char> connection_id_bytes(sizeof(unsigned int));
                unsigned int connection_id_be = htonl(connection_id);
                std::memcpy(connection_id_bytes.data(), &connection_id_be, sizeof(unsigned int));

                // Convert is_download to big-endian and copy
                std::vector<char> packet_size_bytes(sizeof(unsigned int));
                unsigned int packet_size_be = htonl(packet_size);
                std::memcpy(packet_size_bytes.data(), &packet_size_be, sizeof(unsigned int));

                // Convert file_name length to big-endian and copy
                uint32_t file_name_len_be = htonl(file_name.size());
                std::vector<char> file_name_len_bytes(sizeof(uint32_t));
                std::memcpy(file_name_len_bytes.data(), &file_name_len_be, sizeof(uint32_t));

                // Add message prefix for identifying requests than resends
                std::string message_prefix("SE");
                std::vector<char> message_identifier(message_prefix.begin(), message_prefix.end());

                std::vector<std::vector<char>> bytes_list({message_identifier,
                                                           is_download_bytes,
                                                           connection_id_bytes,
                                                           packet_size_bytes,
                                                           file_name_len_bytes,
                                                           std::vector<char>(file_name.begin(), file_name.end())});
                return fup::core::entity::serializer::concatenate_vectors<char>(bytes_list);
            }

            size_t request::deserialize(std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize is_download
                std::memcpy(&is_download, data.data() + offset, sizeof(bool));
                offset += sizeof(bool);

                // Deserialize connection_id
                std::memcpy(&connection_id, data.data() + offset, sizeof(unsigned int));
                connection_id = ntohl(connection_id);
                offset += sizeof(unsigned int);

                // Deserialize packet_size
                std::memcpy(&packet_size, data.data() + offset, sizeof(unsigned int));
                packet_size = ntohl(packet_size);
                offset += sizeof(unsigned int);

                // Deserialize file_name length
                uint32_t fileNameLen;
                std::memcpy(&fileNameLen, data.data() + offset, sizeof(uint32_t));
                fileNameLen = ntohl(fileNameLen);
                offset += sizeof(uint32_t);

                // Deserialize file_name
                file_name.resize(fileNameLen);
                std::memcpy(&file_name[0], data.data() + offset, fileNameLen);
                offset += fileNameLen;

                return offset;
            }
        }
    }
}