#pragma once

#ifndef FUP_CORE_SERVICE_SENDER_HPP
#define FUP_CORE_SERVICE_SENDER_HPP
#include <string>
#include <vector>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <BLAKE3/c/blake3.h>
#include <core/entity/entity.hpp>
#include <core/helper/helper.hpp>
#include <core/interface/checksum.hpp>

namespace fup
{
    namespace core
    {
        namespace service
        {
            class sender
            {
            public:
                int send_key(const std::string key);
                int send_metadata(const entity::metadata &metadata);
                int send_packet(const entity::packet &packet,boost::asio::ip::udp::endpoint &destination);
                int send_request(const entity::request &request);
                int send_resend(const int &package_number);
                int send_ok();
                std::vector<char> *create_checksum(std::vector<char> &data);
                sender(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<fup::core::interface::checksum> checksum);
                ~sender();

            private:
                template <typename T>
                int send_tcp(const T &payload);
                template <typename T>
                int send_udp(const T &payload, boost::asio::ip::udp::endpoint &destination);

                boost::asio::ip::tcp::socket *tcp_socket;
                boost::asio::ip::udp::socket *udp_socket;
                boost::shared_ptr<interface::checksum> checksum_service;
            };
        }
    }
}

#endif