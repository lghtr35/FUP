#pragma once

#ifndef FUP_CORE_ENTITY_SERIALIZER_HPP
#define FUP_CORE_ENTITY_SERIALIZER_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include "serializable.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class serializer
            {
            public:
                template <typename T>
                static std::vector<T> concatenate_vectors(std::vector<std::vector<T>> &vectors)
                {
                    size_t totalLength = std::accumulate(vectors.begin(), vectors.end(), 0,
                                                         [](size_t acc, std::vector<T> &vec)
                                                         { return acc + vec.size(); });

                    std::vector<T> combinedVector(totalLength);

                    for (auto &vec : vectors)
                    {
                        combinedVector.insert(combinedVector.end(), vec.begin(), vec.end());
                    }

                    return combinedVector;
                }

                // Serialization function for string payload
                static std::vector<char> serialize_payload(std::string &p)
                {
                    return std::vector<char>(p.begin(), p.end());
                }

                // Serialization function for entities that are serializable
                template <typename T>
                static std::vector<char> serialize_payload(T &p)
                {
                    return p.serialize();
                }
            };
        }
    }
}

#endif