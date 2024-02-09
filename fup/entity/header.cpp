#pragma once

#include <cstring>
#include <stdexcept>
#include <fup/entity/header.hpp>

namespace fup
{
    namespace entity
    {
        std::vector<uint8_t> header::serialize() const
        {
            std::vector<uint8_t> serializedData(32 + 2 * sizeof(int));

            // Copy the 'checksum' data
            std::memcpy(serializedData.data(), checksum.data(), 32);

            // Copy 'packet_seq_num' and 'body_length'
            std::memcpy(serializedData.data() + 32, &packet_seq_num, sizeof(int));
            std::memcpy(serializedData.data() + 32 + sizeof(int), &body_length, sizeof(int));

            return serializedData;
        }

        size_t header::deserialize(const std::vector<uint8_t> &data)
        {
            if (data.size() < 32 + 2 * sizeof(int))
            {
                throw std::invalid_argument("Given data is not complete for a deserialization");
            }
            // Copy the 'checksum' data
            std::memcpy(checksum.data(), data.data(), 32);

            // Copy 'packet_seq_num' and 'body_length'
            std::memcpy(&packet_seq_num, data.data() + 32, sizeof(int));
            std::memcpy(&body_length, data.data() + 32 + sizeof(int), sizeof(int));

            return 32 + 2 * sizeof(int);
        }
    }
}