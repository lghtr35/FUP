#pragma once
#ifndef FUP_FACADE_SERVER_HPP
#define FUP_FACADE_SERVER_HPP
#include "core/core.hpp"
#include <fstream>
#include <thread>
#include <csignal>
#include "helper/helper.hpp"
#include "manager/file_manager.hpp"
#include "manager/upload_download_manager.hpp"

namespace fup
{
    namespace facade
    {
        class server
        {
        public:
            void run(int thread_count);
            ~server();
            server(int port, manager::file_manager *file_manager_injected = nullptr, fup::core::interface::checksum *checksum_injected = nullptr);
            void handle_accept(fup::core::connection *connection);
            helper::connection_factory *connection_factory;

        private:
            void do_accept(int handler, sockaddr_in client);
            void handle_handshake(fup::core::connection *connection);
            void handle_resend(fup::core::connection *connection);
            void upload_file(fup::core::connection *connection, std::fstream *file);
            void download_file(fup::core::connection *connection, std::fstream *file);

            core::interface::checksum *checksum_service;
            manager::file_manager *file_manager;
            std::vector<std::thread> threads;
            int tcp_socket;
        };
    }
}

#endif