#pragma once

#include <vector>
#include <cstring>
#include <core/entity/metadata.hpp>
#include <core/helper/helper.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> metadata::serialize() const
            {
                // Convert file_package_size to big-endian and copy
                unsigned int packageSizeBE = htonl(file_package_size);
                std::vector<char> packageSizeBytes(sizeof(unsigned int));
                std::memcpy(packageSizeBytes.data(), &packageSizeBE, sizeof(unsigned int));

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
                return helper::serializer::concatenate_vectors<char>({packageSizeBytes,
                                                                      totalSizeBytes,
                                                                      fileNameLenBytes,
                                                                      std::vector<char>(file_name.begin(), file_name.end()),
                                                                      fileExtensionLenBytes,
                                                                      std::vector<char>(file_extension.begin(), file_extension.end())});
            }

            size_t metadata::deserialize(const std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize file_package_size
                std::memcpy(&file_package_size, data.data() + offset, sizeof(unsigned int));
                file_package_size = ntohl(file_package_size);
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
                std::memcpy(file_name.data(), data.data() + offset, fileNameLen);
                offset += fileNameLen;

                // Deserialize file_extension length
                uint32_t fileExtensionLen;
                std::memcpy(&fileExtensionLen, data.data() + offset, sizeof(uint32_t));
                fileExtensionLen = ntohl(fileExtensionLen);
                offset += sizeof(uint32_t);

                // Deserialize file_extension
                file_extension.resize(fileExtensionLen);
                std::memcpy(file_extension.data(), data.data() + offset, fileExtensionLen);
                offset += fileExtensionLen;

                return offset;
            }
        }
    }
}