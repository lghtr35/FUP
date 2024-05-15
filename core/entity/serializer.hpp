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
                static std::vector<T> concatenate_vectors(const std::vector<std::vector<T>> &vectors);
                static std::vector<char> serialize_payload(const std::string &p);
                template <typename T>
                static std::vector<char> serialize_payload(const T &p);
            };
        }
    }
}

#endif