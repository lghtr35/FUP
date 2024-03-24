#pragma once

#include <cstring>
#include <core/entity/packet.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> packet::serialize() const
            {
                std::vector<char> header_bytes = header.serialize();
                return helper::serializer::concatenate_vectors<char>({header_bytes, body});
            }

            size_t packet::deserialize(const std::vector<char> &data)
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
}