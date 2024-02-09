#pragma once

#include <cstring>
#include <fup/entity/packet.hpp>
#include <fup/helper/helper.hpp>

namespace fup
{
    namespace entity
    {
        std::vector<uint8_t> packet::serialize() const
        {
            std::vector<uint8_t> header_bytes = header.serialize();
            return helper::serializer::concatenate_vectors<uint8_t>({header_bytes, body});
        }

        size_t packet::deserialize(const std::vector<uint8_t> &data)
        {
            size_t offset = header.deserialize(data);
            // Resize the body vector to hold the remaining data
            body.resize(data.size() - offset);
            // Copy the remaining data into the body vector
            std::memcpy(body.data(), data.data() + offset, body.size());

            return offset + body.size();
        }
    }
}