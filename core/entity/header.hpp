#pragma once

#ifndef FUP_CORE_ENTITY_HEADER_HPP
#define FUP_CORE_ENTITY_HEADER_HPP
#include "serializable.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class header : public fup::core::entity::serializable
            {
            public:
                header(){};
                ~header() = default;
                std::vector<char> checksum; // 32 bytes
                int package_seq_num;        // 4 bytes
                int body_length;            // 4 bytes
                std::vector<char> serialize() const override;
                size_t deserialize(const std::vector<char> &data) override;
            };
        }
    }
}
#endif