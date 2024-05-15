#pragma once
#ifndef FUP_CORE_SERVICE_BLAKE3_CHECKSUM_HPP
#define FUP_CORE_SERVICE_BLAKE3_CHECKSUM_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <mutex>
#include <BLAKE3/c/blake3.h>
#include "interface/checksum.hpp"
#include "interface/constants.hpp"
#include "core/entity/serializable.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            class blake3_checksum : public interface::checksum
            {
            public:
                blake3_checksum();
                ~blake3_checksum();
                bool validate_checksum(std::vector<char> &data, std::vector<char> &checksum);
                std::vector<char> create_checksum(std::vector<char> &data);

            private:
                blake3_hasher *hasher;
                std::mutex mutex;
            };
        }
    }
}
#endif