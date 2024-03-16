#pragma once

#ifndef FUP_CORE_SENDER_HPP
#define FUP_CORE_SENDER_HPP
#include <string>
#include <vector>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <BLAKE3/c/blake3.h>
#include <core/entity/entity.hpp>
#include <core/helper/helper.hpp>

namespace fup
{
    namespace core
    {
        class sender
        {
        public:
            int send_key(const std::string key);
            int send_metadata(const entity::metadata &metadata);
            int send_packet(const entity::packet &packet);
            int send_request(const entity::request &request);
            int send_resend(const int &package_number);
            int send_ok();
            std::vector<uint8_t> *create_checksum(std::vector<uint8_t> &data);
            sender(int port);
            ~sender();

        private:
            template <typename T>
            int send_tcp(const T &payload);
            template <typename T>
            int send_udp(const T &payload);

            fup::core::helper::socket_singleton_factory *socket_factory;
            blake3_hasher *hasher;
        };
    }
}

#endif