#pragma once
#ifndef FUP_CONNECTION_MANAGER_HPP
#define FUP_CONNECTION_MANAGER_HPP
#include <core/core.hpp>
#include "../helper/helper.hpp"
#include "file_manager.hpp"

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            class connection_manager
            {
            public:
                // Client
                void connect();
                void init_handshake();
                void client_upload_file();
                void client_download_file();
                // House Keeping
                connection_manager();
                ~connection_manager();

            private:
                manager::file_manager *file_manager;
                helper::connection_factory *connection_factory;
                boost::asio::ip::tcp::acceptor *acceptor;
                unsigned int max_connections;
            };
        }
    }
}

#endif