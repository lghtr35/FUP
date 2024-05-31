#pragma once

#ifndef FUP_CORE_ENTITY_RESPONSE_HPP
#define FUP_CORE_ENTITY_RESPONSE_HPP
#include "serializable.hpp"
#include "serializer.hpp"
#include <string>

namespace fup
{
    namespace core
    {
        namespace entity
        {
            class response : public fup::core::entity::serializable
            {
            public:
                response(){};
                unsigned int status;
                unsigned int udp_port;
                unsigned int connection_id;
                std::vector<char> serialize() override;
                size_t deserialize(std::vector<char> &data) override;
            };
        }
    }
}

#endif