#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <BLAKE3/c/blake3.h>
#include <core/helper/constants.hpp>
#include <core/entity/serializable.hpp>

namespace fup
{
    namespace core
    {
        namespace helper
        {
            class checksum
            {
            public:
                static bool validate_checksum(blake3_hasher *hasher, std::vector<uint8_t> &data, std::vector<uint8_t> &checksum)
                {
                    // Calculate the checksum for the data
                    blake3_hasher_init(hasher);
                    blake3_hasher_update(hasher, data.data(), data.size());
                    std::vector<uint8_t> calculated_checksum(BLAKE3_OUT_LEN);
                    blake3_hasher_finalize(hasher, calculated_checksum.data(), BLAKE3_OUT_LEN);

                    // Compare the calculated checksum with the provided checksum
                    return std::memcmp(calculated_checksum.data(), checksum.data(), BLAKE3_OUT_LEN) == 0;
                }

                static std::vector<uint8_t> *create_checksum(blake3_hasher *hasher, std::vector<uint8_t> &data)
                {
                    // Calculate the checksum for the data
                    blake3_hasher_init(hasher);
                    blake3_hasher_update(hasher, data.data(), data.size());
                    std::vector<uint8_t> checksum(BLAKE3_OUT_LEN);
                    blake3_hasher_finalize(hasher, checksum.data(), BLAKE3_OUT_LEN);
                    return &checksum;
                }
            };
        }
    }
}