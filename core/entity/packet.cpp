
#include "packet.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> packet::serialize()
            {
                std::vector<char> header_bytes = header.serialize();
                std::vector<std::vector<char>> bytes_list({header_bytes, body});
                return fup::core::entity::serializer::concatenate_vectors<char>(bytes_list);
            }

            size_t packet::deserialize(std::vector<char> &data)
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