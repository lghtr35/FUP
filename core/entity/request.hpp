#pragma once

#ifndef FUP_CORE_ENTITY_REQUEST_HPP
#define FUP_CORE_ENTITY_REQUEST_HPP
#include <vector>
#include <string>
#include <cstring>
#include <core/entity/header.hpp>
#include <core/entity/serializable.hpp>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class request : fup::core::entity::serializable
            {
            public:
                request(){};
                std::string file_name;
                bool is_download;
                unsigned int packet_size;
                std::vector<char> serialize() const override;
                size_t deserialize(const std::vector<char> &data) override;
            };
        }
    }
}
#endif