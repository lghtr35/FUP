#pragma once
#ifndef FUP_CORE_CONNECTION_HPP
#define FUP_CORE_CONNECTION_HPP

#include "service/sender.hpp"
#include "service/receiver.hpp"
#include "helper/helper.hpp"

namespace fup
{
    namespace core
    {
        class connection
        {
        public:
            connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<interface::checksum> checksum, unsigned int idx);
            ~connection();
            service::sender *get_sender_service();
            service::receiver *get_receiver_service();
            boost::asio::ip::tcp::socket *get_tcp_socket();
            boost::asio::ip::udp::socket *get_udp_socket();
            unsigned int get_id();

        private:
            service::receiver *receiver_service;
            service::sender *sender_service;
            boost::asio::ip::tcp::socket *tcp_socket;
            boost::asio::ip::udp::socket *udp_socket;
            unsigned int id;
        };
    }
}

#endif