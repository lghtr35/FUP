#pragma once

#ifndef FUP_CORE_RECEIVER_HPP
#define FUP_CORE_RECEIVER_HPP
#include <core/entity/entity.hpp>
#include <core/helper/helper.hpp>
#include <iostream>
#include <BLAKE3/c/blake3.h>
#include <boost/asio.hpp>

namespace fup
{
    namespace core
    {
        class receiver
        {
        public:
            fup::core::entity::request *receive_request();
            fup::core::entity::packet *receive_packet(unsigned short &fps);
            bool receive_ok();
            int receive_resend();
            fup::core::entity::metadata *receive_metadata();
            std::string receive_key();
            bool validate_checksum(std::vector<uint8_t> &data, std::vector<uint8_t> &checksum);
            std::vector<uint8_t> *create_checksum(std::vector<uint8_t> &data);
            receiver();
            ~receiver(){};

        private:
            fup::core::entity::metadata *metadata;
            blake3_hasher *hasher;
            fup::core::helper::socket_singleton_factory *socket_factory;
        };
    }
}
#endif