#pragma once
#ifndef FUP_FACADE_CLIENT_HPP
#define FUP_FACADE_CLIENT_HPP
#include "core/core.hpp"
#include <fstream>
#include "helper/helper.hpp"
#include "manager/file_manager.hpp"
#include "manager/upload_download_manager.hpp"

namespace fup
{
    namespace facade
    {
        class client
        {
        public:
            void upload_file(std::string address, std::string path_to_file, int packet_size);
            void download_file(std::string address, std::string file_name, int packet_size);
            ~client();
            client(std::string file_location, manager::file_manager *file_manager_injected = nullptr, fup::core::interface::checksum *checksum_injected = nullptr);

        private:
            fup::core::connection *connect_to_remote(std::string address, std::string file_name, bool is_download, int packet_size);
            void initiate_handshake(fup::core::connection *connection, std::string file_name, bool is_download, int packet_size);
            void handle_resend(fup::core::connection *connection);

            core::interface::checksum *checksum_service;
            manager::file_manager *file_manager;
            helper::connection_factory *connection_factory;
            int tcp_socket;
            int udp_socket;
        };
    }
}

#endif