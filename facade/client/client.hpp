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
            void connect(std::string address, unsigned int port);
            void initiate_handshake(fup::core::connection *connection);
            void handle_resend(fup::core::connection *connection);
            void upload_file(fup::core::connection *connection, std::fstream *file);
            void download_file(fup::core::connection *connection, std::fstream *file);
            ~client();
            client(std::string file_location, int port);

        private:
            core::interface::checksum *checksum_service;
            manager::file_manager *file_manager;
            helper::connection_factory *connection_factory;
            boost::asio::ip::tcp::socket tcp_socket;
            boost::asio::ip::udp::socket udp_socket;
            boost::asio::io_context io_context;
        };
    }
}

#endif