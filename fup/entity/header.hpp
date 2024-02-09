#pragma once

#ifndef FUP_ENTITY_HEADER_HPP
#define FUP_ENTITY_HEADER_HPP
#include <fup/abstraction/serializable.hpp>

namespace fup
{
    namespace entity
    {
        class header : abstraction::serializable
        {
        public:
            std::vector<uint8_t> checksum; // 32 bytes
            int packet_seq_num;            // 4 bytes
            int body_length;               // 4 bytes
            std::vector<uint8_t> serialize() const override;
            size_t deserialize(const std::vector<uint8_t> &data) override;
        };
    }
}
#endif