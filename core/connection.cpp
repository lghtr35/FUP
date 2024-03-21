#pragma once
#include "connection.hpp"

namespace fup
{
    namespace core
    {
        connection::connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<core::interface::checksum> checksum, unsigned int idx)
        {
            receiver_service = new fup::core::service::receiver(tcp, udp, checksum);
            sender_service = new fup::core::service::sender(tcp, udp, checksum);
            tcp_socket = tcp;
            udp_socket = udp;
            id = idx;
        }

        connection::~connection()
        {
            delete receiver_service;
            delete sender_service;
            delete tcp_socket;
            delete udp_socket;
        }

        fup::core::service::receiver *connection::get_receiver_service()
        {
            return receiver_service;
        }

        fup::core::service::sender *connection::get_sender_service()
        {
            return sender_service;
        }

        unsigned int connection::get_id()
        {
            return id;
        }
    }
}