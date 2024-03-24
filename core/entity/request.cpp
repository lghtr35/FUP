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
                std::vector<char> packetSizeBytes(sizeof(unsigned int));
                unsigned int packetSizeBE = htonl(packet_size);
                std::memcpy(packetSizeBytes.data(), &packetSizeBE, sizeof(unsigned int));

                // Convert is_download to big-endian and copy
                std::vector<char> isDownloadBytes(sizeof(bool));
                std::memcpy(isDownloadBytes.data(), &is_download, sizeof(bool));

                // Convert file_name length to big-endian and copy
                uint32_t fileNameLenBE = htonl(file_name.size());
                std::vector<char> fileNameLenBytes(sizeof(uint32_t));
                std::memcpy(fileNameLenBytes.data(), &fileNameLenBE, sizeof(uint32_t));

                return helper::serializer::concatenate_vectors<char>({isDownloadBytes,
                                                                      fileNameLenBytes,
                                                                      std::vector<char>(file_name.begin(), file_name.end())});
            }

            size_t request::deserialize(const std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize is_download
                std::memcpy(&is_download, data.data() + offset, sizeof(bool));
                offset += sizeof(bool);

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
                std::memcpy(file_name.data(), data.data() + offset, fileNameLen);
                offset += fileNameLen;

                return offset;
            }
        }
    }
}