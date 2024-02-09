#pragma once

#ifndef FUP_ENTITY_PACKET_HPP
#define FUP_ENTITY_PACKET_HPP
#include <fup/abstraction/serializable.hpp>
#include <fup/entity/header.hpp>
#include <vector>

namespace fup
{
    namespace entity
    {
        class packet : abstraction::serializable
        {
        public:
            header header;             // Fixed size
            std::vector<uint8_t> body; // Variable amount of bytes. Will be preconfigured on server and client connection
            std::vector<uint8_t> serialize() const override;
            size_t deserialize(const std::vector<uint8_t> &data) override;
        };
    }
}
#endif