#pragma once
#ifndef FUP_FACADE_SERVER_HPP
#define FUP_FACADE_SERVER_HPP
#include <core/core.hpp>
#include "./helper/helper.hpp"
#include "./manager/file_manager.hpp"

namespace fup
{
    namespace facade
    {
        class server
        {
        public:
            void run(int thread_count);
            void listen();
            void handle_accept(fup::core::connection *connection, const boost::system::error_code &error);
            void handle_handshake(fup::core::connection *connection);
            void upload_file(fup::core::connection *connection);
            void download_file(fup::core::connection *connection);
            ~server();
            server();

        private:
            manager::file_manager *file_manager;
            helper::connection_factory *connection_factory;
            helper::socket_factory *socket_factory;
            boost::asio::ip::tcp::acceptor *acceptor;
            boost::asio::ip::udp::resolver *resolver;
        };
    }
}

#endif