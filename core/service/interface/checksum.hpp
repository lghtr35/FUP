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
                virtual ~checksum() = 0;
                virtual bool validate_checksum(std::vector<char> &data, std::vector<char> &checksum) = 0;
                virtual std::vector<char> create_checksum(std::vector<char> &data) = 0;
            };
        }
    }
}
#endif