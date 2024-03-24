#pragma once

#ifndef FUP_CORE_ENTITY_SERIALIZABLE_HPP
#define FUP_CORE_ENTITY_SERIALIZABLE_HPP
#include <vector>
namespace fup
{
    namespace core
    {
        namespace entity
        {
            class serializable
            {
            public:
                virtual std::vector<char> serialize() const = 0;
                virtual size_t deserialize(const std::vector<char> &data) = 0;
                virtual ~serializable() = default;
            };
        }
    }
}
#endif