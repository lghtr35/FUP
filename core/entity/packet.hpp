#pragma once

#ifndef FUP_CORE_ENTITY_PACKET_HPP
#define FUP_CORE_ENTITY_PACKET_HPP
#include <vector>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class packet : serializable
            {
            public:
                packet(){};
                explicit packet(header h) : header(h){};
                ~packet() = default;
                header header;          // Fixed size
                std::vector<char> body; // Variable amount of bytes. Will be preconfigured on server and client connection
                std::vector<char> serialize() const override;
                size_t deserialize(const std::vector<char> &data) override;
            };
        }
    }
}
#endif