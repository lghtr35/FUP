#pragma once

#include <core/entity/request.hpp>
#include <core/helper/helper.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<uint8_t> request::serialize() const
            {
                // Convert is_download to big-endian and copy
                std::vector<uint8_t> isDownloadBytes(sizeof(bool));
                std::memcpy(isDownloadBytes.data(), &is_download, sizeof(unsigned int));

                // Convert file_name length to big-endian and copy
                uint32_t fileNameLenBE = htonl(file_name.size());
                std::vector<uint8_t> fileNameLenBytes(sizeof(uint32_t));
                std::memcpy(fileNameLenBytes.data(), &fileNameLenBE, sizeof(uint32_t));

                return helper::serializer::concatenate_vectors<uint8_t>({isDownloadBytes,
                                                                         fileNameLenBytes,
                                                                         std::vector<uint8_t>(file_name.begin(), file_name.end())});
            }

            size_t request::deserialize(const std::vector<uint8_t> &data)
            {
                size_t offset = 0;

                // Deserialize is_download
                std::memcpy(&is_download, data.data() + offset, sizeof(bool));
                offset += sizeof(bool);

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