#pragma once
#ifndef FUP_CORE_SERVICE_RECEIVER_HPP
#define FUP_CORE_SERVICE_RECEIVER_HPP
#include "core/entity/entity.hpp"
#include <iostream>
#include <BLAKE3/c/blake3.h>
#include "interface/constants.hpp"
#include "imports.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            class receiver
            {
            public:
                std::string receive_message_identifier();
                fup::core::entity::request receive_request();
                fup::core::entity::packet receive_packet();
                fup::core::entity::response receive_response();
                std::pair<unsigned int, unsigned int> receive_resend();
                fup::core::entity::metadata receive_metadata();
                std::string receive_key();
                receiver(int &tcp, int &udp);
                ~receiver();

            private:
                fup::core::entity::metadata metadata;
                int tcp_socket;
                int udp_socket;
            };
        }
    }
}
#endif