
#include "metadata.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> metadata::serialize()
            {
                // Convert file_packet_size to big-endian and copy
                unsigned int packetSizeBE = htonl(file_packet_size);
                std::vector<char> packetSizeBytes(sizeof(unsigned int));
                std::memcpy(packetSizeBytes.data(), &packetSizeBE, sizeof(unsigned int));

                // Convert file_total_size to big-endian and copy
                unsigned int totalSizeBE = htonl(file_total_size);
                std::vector<char> totalSizeBytes(sizeof(unsigned int));
                std::memcpy(totalSizeBytes.data(), &totalSizeBE, sizeof(unsigned int));

                // Convert file_name length to big-endian and copy
                uint32_t fileNameLenBE = htonl(file_name.size());
                std::vector<char> fileNameLenBytes(sizeof(uint32_t));
                std::memcpy(fileNameLenBytes.data(), &fileNameLenBE, sizeof(uint32_t));

                // Convert file_extension length to big-endian and copy
                uint32_t fileExtensionLenBE = htonl(file_extension.size());
                std::vector<char> fileExtensionLenBytes(sizeof(uint32_t));
                std::memcpy(fileExtensionLenBytes.data(), &fileExtensionLenBE, sizeof(uint32_t));

                // Concatenate all byte vectors
                std::vector<std::vector<char>> bytes_list({packetSizeBytes, totalSizeBytes, fileNameLenBytes, std::vector<char>(file_name.begin(), file_name.end()), fileExtensionLenBytes, std::vector<char>(file_extension.begin(), file_extension.end())});
                return fup::core::entity::serializer::concatenate_vectors<char>(bytes_list);
            }

            size_t metadata::deserialize(std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize file_packet_size
                std::memcpy(&file_packet_size, data.data() + offset, sizeof(unsigned int));
                file_packet_size = ntohl(file_packet_size);
                offset += sizeof(unsigned int);

                // Deserialize file_total_size
                std::memcpy(&file_total_size, data.data() + offset, sizeof(unsigned int));
                file_total_size = ntohl(file_total_size);
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

                // Deserialize file_extension length
                uint32_t fileExtensionLen;
                std::memcpy(&fileExtensionLen, data.data() + offset, sizeof(uint32_t));
                fileExtensionLen = ntohl(fileExtensionLen);
                offset += sizeof(uint32_t);

                // Deserialize file_extension
                file_extension.resize(fileExtensionLen);
                std::memcpy(&file_extension[0], data.data() + offset, fileExtensionLen);
                offset += fileExtensionLen;

                return offset;
            }
        }
    }
}