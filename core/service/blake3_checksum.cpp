

#include "blake3_checksum.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            blake3_checksum::blake3_checksum()
            {
                blake3_hasher_init(hasher);
            }

            blake3_checksum::~blake3_checksum()
            {
                delete hasher;
            }

            bool blake3_checksum::validate_checksum(std::vector<char> &data, std::vector<char> &checksum)
            {
                mutex.lock();
                // Calculate the checksum for the data
                blake3_hasher_update(hasher, data.data(), data.size());
                std::vector<uint8_t> calculated_checksum(BLAKE3_OUT_LEN);
                blake3_hasher_finalize(hasher, calculated_checksum.data(), BLAKE3_OUT_LEN);
                mutex.unlock();
                // Compare the calculated checksum with the provided checksum
                return std::memcmp(calculated_checksum.data(), checksum.data(), BLAKE3_OUT_LEN) == 0;
            }

            std::vector<char> blake3_checksum::create_checksum(std::vector<char> &data)
            {
                mutex.lock();
                // Calculate the checksum for the data
                blake3_hasher_update(hasher, data.data(), data.size());
                std::vector<uint8_t> checksum(BLAKE3_OUT_LEN);
                blake3_hasher_finalize(hasher, checksum.data(), BLAKE3_OUT_LEN);
                std::vector<char> res;
                res.resize(checksum.size());
                for (int i = 0; i < checksum.size(); i++)
                {
                    res[i] = (char)checksum[i];
                }
                mutex.unlock();
                return res;
            }
        };
    }
}
