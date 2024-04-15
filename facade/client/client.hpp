#pragma once
#ifndef FUP_FACADE_CLIENT_HPP
#define FUP_FACADE_CLIENT_HPP
#include <core/core.hpp>
#include <fstream>
#include "./helper/helper.hpp"
#include "./manager/file_manager.hpp"

namespace fup
{
    namespace facade
    {
        class client
        {
        public:
            ~client();
            client();

        private:
            core::interface::checksum *checksum_service;
            manager::file_manager *file_manager;
            helper::connection_factory *connection_factory;
            helper::socket_factory *socket_factory;
            boost::asio::ip::tcp::acceptor *acceptor;
            boost::asio::ip::udp::resolver *resolver;
            boost::asio::ip::udp::socket *udp_socket;
            boost::asio::io_context io_context;
        };
    }
}

#endif