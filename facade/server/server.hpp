#pragma once
#ifndef FUP_FACADE_SERVER_HPP
#define FUP_FACADE_SERVER_HPP
#include <core/core.hpp>
#include <fstream>
#include <thread>
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
            void do_accept();
            void handle_accept(fup::core::connection *connection, const boost::system::error_code &error);
            void handle_handshake(fup::core::connection *connection);
            void handle_resend(fup::core::connection *connection);
            void upload_file(fup::core::connection *connection, std::fstream *file);
            void download_file(fup::core::connection *connection, std::fstream *file);
            ~server();
            server(std::string file_location, int port);

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