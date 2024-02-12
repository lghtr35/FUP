#pragma once

#include <cstring>
#include <stdexcept>
#include <core/entity/header.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<uint8_t> header::serialize() const
            {
                std::vector<uint8_t> serializedData(32 + 2 * sizeof(int));

                // Copy the 'checksum' data
                std::memcpy(serializedData.data(), checksum.data(), 32);

                // Convert 'packet_seq_num' to big-endian and copy
                int packetSeqNumBE = htonl(packet_seq_num);
                std::memcpy(serializedData.data() + 32, &packetSeqNumBE, sizeof(int));

                // Convert 'body_length' to big-endian and copy
                int bodyLengthBE = htonl(body_length);
                std::memcpy(serializedData.data() + 32 + sizeof(int), &bodyLengthBE, sizeof(int));

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

                // Copy 'packet_seq_num' and 'body_length' and convert to host endian
                std::memcpy(&packet_seq_num, data.data() + 32, sizeof(int));
                packet_seq_num = ntohl(packet_seq_num);

                std::memcpy(&body_length, data.data() + 32 + sizeof(int), sizeof(int));
                body_length = ntohl(body_length);

                return 32 + 2 * sizeof(int);
            }
        }
    }
}
