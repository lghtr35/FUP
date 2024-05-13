#pragma once

#ifndef FUP_CORE_ENTITY_PACKAGE_HPP
#define FUP_CORE_ENTITY_PACKAGE_HPP
#include <vector>
#include "header.hpp"
#include <core/helper/helper.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class package : serializable
            {
            public:
                package(){};
                explicit package(header h, std::vector<char> b) : header(h), body(b){};
                ~package() = default;
                header header;          // Fixed size
                std::vector<char> body; // Variable amount of bytes. Will be preconfigured on server and client connection
                std::vector<char> serialize() const override;
                size_t deserialize(const std::vector<char> &data) override;
            };
        }
    }
}
#endif