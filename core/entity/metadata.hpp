#pragma once

#ifndef FUP_CORE_ENTITY_METADATA_HPP
#define FUP_CORE_ENTITY_METADATA_HPP
#include <string>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class metadata : serializable
            {
            public:
                metadata(){};
                explicit metadata(unsigned int tp, unsigned short fps, unsigned int fts, std::string fn, std::string fe) : file_package_size(fps), file_total_size(fts), file_name(fn), file_extension(fe){};
                unsigned int tcp_port;            // 4 bytes
                unsigned short file_package_size; // 2 bytes
                unsigned int file_total_size;     // 4 bytes
                std::string file_name;            // Unknown amount of bytes
                std::string file_extension;       // Unknown amount of bytes
                std::vector<uint8_t> serialize() const override;
                size_t deserialize(const std::vector<uint8_t> &data) override;
            };
        }
    }
}
#endif