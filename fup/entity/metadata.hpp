#pragma once

#ifndef FUP_ENTITY_METADATA_HPP
#define FUP_ENTITY_METADATA_HPP
#include <fup/abstraction/serializable.hpp>
#include <string>

namespace fup
{
    namespace entity
    {
        class metadata : abstraction::serializable
        {
        public:
            unsigned short file_package_size; // 2 bytes
            unsigned int file_total_size;     // 4 bytes
            std::string file_name;            // Unknown amount of bytes
            std::string file_extension;       // Unknown amount of bytes
            std::vector<uint8_t> serialize() const override;
            size_t deserialize(const std::vector<uint8_t> &data) override;
        };
    }
}
#endif