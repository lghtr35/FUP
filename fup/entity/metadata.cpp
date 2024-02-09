#pragma once

#include <vector>
#include <cstring>
#include <fup/entity/metadata.hpp>
#include <fup/helper/helper.hpp>

namespace fup
{
    namespace entity
    {
        std::vector<uint8_t> metadata::serialize() const
        {
            // Convert file_package_size and file_total_size to bytes
            std::vector<uint8_t> file_package_size_bytes(sizeof(unsigned short));
            std::memcpy(file_package_size_bytes.data(), &file_package_size, sizeof(unsigned short));

            std::vector<uint8_t> file_total_size_bytes(sizeof(unsigned int));
            std::memcpy(file_total_size_bytes.data(), &file_total_size, sizeof(unsigned int));

            // Convert file_name to bytes
            std::vector<uint8_t> file_name_bytes(file_name.begin(), file_name.end());

            // Convert file_extension to bytes
            std::vector<uint8_t> file_extension_bytes(file_extension.begin(), file_extension.end());

            // Convert lengths to bytes
            std::vector<uint8_t> file_name_length_bytes(sizeof(uint32_t));
            uint32_t file_name_size = file_name_bytes.size();
            std::memcpy(file_name_length_bytes.data(), &file_name_size, sizeof(uint32_t));

            std::vector<uint8_t> file_extension_length_bytes(sizeof(uint32_t));
            uint32_t file_extension_size = file_extension_bytes.size();
            std::memcpy(file_extension_length_bytes.data(), &file_extension_size, sizeof(uint32_t));

            // Concatenate all byte vectors
            return helper::serializer::concatenate_vectors<uint8_t>({file_package_size_bytes,
                                                                     file_total_size_bytes,
                                                                     file_name_length_bytes,
                                                                     file_name_bytes,
                                                                     file_extension_length_bytes,
                                                                     file_extension_bytes});
        }

        size_t metadata::deserialize(const std::vector<uint8_t> &data)
        {
            size_t offset = 0;

            // Deserialize file_package_size
            std::memcpy(&file_package_size, data.data() + offset, sizeof(unsigned short));
            offset += sizeof(unsigned short);

            // Deserialize file_total_size
            std::memcpy(&file_total_size, data.data() + offset, sizeof(unsigned int));
            offset += sizeof(unsigned int);

            // Deserialize file_name length
            uint32_t file_nameLen;
            std::memcpy(&file_nameLen, data.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            // Deserialize file_name
            file_name.resize(file_nameLen);
            std::memcpy(file_name.data(), data.data() + offset, file_nameLen);
            offset += file_nameLen;

            // Deserialize file_extension length
            uint32_t file_extensionLen;
            std::memcpy(&file_extensionLen, data.data() + offset, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            // Deserialize file_extension
            file_extension.resize(file_extensionLen);
            std::memcpy(file_extension.data(), data.data() + offset, file_extensionLen);
            offset += file_extensionLen;

            return offset;
        }
    }
}