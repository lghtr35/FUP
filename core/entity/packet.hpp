#pragma once

#ifndef FUP_CORE_ENTITY_PACKET_HPP
#define FUP_CORE_ENTITY_PACKET_HPP
#include <vector>
#include <cstring>
#include "serializer.hpp"
#include "header.hpp"
#include "serializable.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class packet : public fup::core::entity::serializable
            {
            public:
                packet(){};
                explicit packet(header h, std::vector<char> b) : header(h), body(b){};
                ~packet() = default;
                header header;          // Fixed size
                std::vector<char> body; // Variable amount of bytes. Will be preconfigured on server and client connection
                std::vector<char> serialize() override;
                size_t deserialize(std::vector<char> &data) override;
            };
        }
    }
}
#endif