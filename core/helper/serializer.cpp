#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <core/entity/serializable.hpp>

namespace fup
{
    namespace core
    {
        namespace helper
        {
            class serializer
            {
            public:
                template <typename T>
                static std::vector<T> concatenate_vectors(const std::vector<std::vector<T>> &vectors)
                {
                    size_t totalLength = std::accumulate(vectors.begin(), vectors.end(), 0,
                                                         [](size_t acc, const std::vector<T> &vec)
                                                         { return acc + vec.size(); });

                    std::vector<T> combinedVector(totalLength);

                    for (const auto &vec : vectors)
                    {
                        combinedVector.insert(combinedVector.end(), vec.begin(), vec.end());
                    }

                    return combinedVector;
                }

                // Serialization function for string payload
                static std::vector<char> serialize_payload(const std::string &p)
                {
                    return std::vector<char>(p.begin(), p.end());
                }

                // Serialization function for entities that are serializable
                template <typename T, typename = std::enable_if_t<std::is_base_of_v<fup::core::entity::serializable, T>>>
                static std::vector<char> serialize_payload(const T &p)
                {
                    return p.serialize();
                }
            };
        }
    }
}
