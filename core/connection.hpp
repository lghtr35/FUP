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

        private:
            service::receiver *receiver_service;
            service::sender *sender_service;
            unsigned int id;
        };
    }
}

#endif