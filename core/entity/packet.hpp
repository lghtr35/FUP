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
                header header;             // Fixed size
                std::vector<uint8_t> body; // Variable amount of bytes. Will be preconfigured on server and client connection
                std::vector<uint8_t> serialize() const override;
                size_t deserialize(const std::vector<uint8_t> &data) override;
            };
        }
    }
}
#endif