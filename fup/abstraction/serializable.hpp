#pragma once

#ifndef FUP_ABSTRACTION_SERIALIZABLE_HPP
#define FUP_ABSTRACTION_SERIALIZABLE_HPP
#include <vector>
namespace fup
{
    namespace abstraction
    {
        class serializable
        {
        public:
            virtual std::vector<uint8_t> serialize() const = 0;
            virtual size_t deserialize(const std::vector<uint8_t> &data) = 0;
            virtual ~serializable() = default;
        };
    }
}
#endif