#pragma once

#ifndef FUP_CORE_INTERFACE_CHECKSUM_HPP
#define FUP_CORE_INTERFACE_CHECKSUM_HPP
#include <vector>

namespace fup
{
    namespace core
    {
        namespace interface
        {
            class checksum
            {
            public:
                virtual bool validate_checksum(std::vector<uint8_t> &data, std::vector<uint8_t> &checksum) = 0;
                virtual std::vector<uint8_t> *create_checksum(std::vector<uint8_t> &data) = 0;
            };
        }
    }
}
#endif