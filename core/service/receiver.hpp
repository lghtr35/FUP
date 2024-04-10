#pragma once
#ifndef FUP_CORE_SERVICE_RECEIVER_HPP
#define FUP_CORE_SERVICE_RECEIVER_HPP
#include <core/entity/entity.hpp>
#include <core/helper/helper.hpp>
#include <iostream>
#include <BLAKE3/c/blake3.h>
#include <boost/asio.hpp>
#include <core/interface/checksum.hpp>

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
                fup::core::entity::request *receive_request();
                fup::core::entity::packet *receive_packet();
                fup::core::entity::response *receive_response();
                std::pair<int, int> receive_resend();
                fup::core::entity::metadata *receive_metadata();
                std::string receive_key();
                receiver(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<fup::core::interface::checksum> checksum);
                ~receiver(){};

            private:
                boost::shared_ptr<interface::checksum> checksum_service;
                fup::core::entity::metadata *metadata;
                boost::asio::ip::tcp::socket *tcp_socket;
                boost::asio::ip::udp::socket *udp_socket;
            };
        }
    }
}
#endif