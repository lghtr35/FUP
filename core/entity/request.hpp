#pragma once

#ifndef FUP_CORE_ENTITY_REQUEST_HPP
#define FUP_CORE_ENTITY_REQUEST_HPP
#include <vector>
#include <string>
#include <cstring>
#include "serializable.hpp"
#include "serializer.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class request : public fup::core::entity::serializable
            {
            public:
                request(){};
                std::string file_name;
                bool is_download;
                unsigned int connection_id;
                unsigned int packet_size;
                std::vector<char> serialize() override;
                size_t deserialize(std::vector<char> &data) override;
            };
        }
    }
}
#endif