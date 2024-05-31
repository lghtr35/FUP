#pragma once

#ifndef FUP_CORE_ENTITY_METADATA_HPP
#define FUP_CORE_ENTITY_METADATA_HPP
#include <string>
#include <vector>
#include <cstring>
#include "serializer.hpp"
#include "serializable.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class metadata : public fup::core::entity::serializable
            {
            public:
                metadata(){};
                explicit metadata(unsigned short fps, unsigned int fts, std::string fn, std::string fe) : file_packet_size(fps), file_total_size(fts), file_name(fn), file_extension(fe){};
                unsigned int file_packet_size; // 4 bytes
                unsigned int file_total_size;  // 4 bytes
                std::string file_name;         // Unknown amount of bytes
                std::string file_extension;    // Unknown amount of bytes
                std::vector<char> serialize() override;
                size_t deserialize(std::vector<char> &data) override;
            };
        }
    }
}
#endif