#pragma once

#ifndef FUP_CORE_SERVICE_SENDER_HPP
#define FUP_CORE_SERVICE_SENDER_HPP
#include <string>
#include <vector>
#include <iostream>
#include <BLAKE3/c/blake3.h>
#include "core/entity/entity.hpp"
#include "imports.hpp"

namespace fup
{
    namespace core
    {
        namespace service
        {
            class sender
            {
            public:
                int send_key(std::string key);
                int send_metadata(entity::metadata &metadata);
                int send_packet(entity::packet &packet);
                int send_request(entity::request &request);
                int send_response(entity::response &response);
                int send_resend(unsigned int connection_id, unsigned int packet_number);
                sender(int tcp, int udp);
                ~sender();

            private:
                template <typename T>
                int send_tcp(T &payload);
                template <typename T>
                int send_udp(T &payload);

                int tcp_socket;
                int udp_socket;
            };
        }
    }
}

#endif