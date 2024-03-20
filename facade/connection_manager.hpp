#pragma once
#ifndef FUP_CONNECTION_MANAGER_HPP
#define FUP_CONNECTION_MANAGER_HPP
#include <core/core.hpp>
#include "helper/helper.hpp"

namespace fup
{
    namespace facade
    {
        class connection_manager
        {
        public:
            // Server
            void listen();
            void handle_handshake();
            void send_file();
            // Client
            void init_handshake();
            void handle_file();
            // House Keeping
            connection_manager();
            ~connection_manager();

        private:
            helper::connection_factory *connection_factory;
            boost::asio::ip::tcp::acceptor *acceptor;
            unsigned int max_connections;
        };
    }
}

#endif