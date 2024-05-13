#pragma once
#include "client.hpp"

namespace fup
{
    namespace facade
    {
        fup::core::connection *client::connect(std::string address, std::string file_name, bool is_download, int package_size)
        {
            fup::core::connection *connection = connection_factory->get_connection(&tcp_socket, &udp_socket);

            boost::asio::ip::tcp::resolver resolver(io_context);
            boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(address, "fup");

            boost::asio::connect(*connection->get_tcp_socket(), endpoints);
            return connection;
        }

        void client::initiate_handshake(fup::core::connection *connection, std::string file_name, bool is_download, int package_size)
        {
            fup::core::entity::request req;
            req.connection_id = connection->get_id();
            req.file_name = file_name;
            req.is_download = is_download;
            req.package_size = package_size;
            req.udp_port = connection->get_udp_socket()->local_endpoint().port();

            connection->get_sender_service()->send_request(req);
            fup::core::entity::response *resp = connection->get_receiver_service()->receive_response();
            if (resp->status < 0)
            {
                std::string error_msg = std::string("Response status: ") + std::to_string(resp->status);
                throw new std::runtime_error(error_msg);
            }
            connection->set_remote_connection_id(resp->connection_id);
            connection->set_remote_udp_port(resp->udp_port);
        }

        void client::handle_resend(fup::core::connection *connection)
        {
            std::pair<int, int> pair = connection->get_receiver_service()->receive_resend();
            int connection_id = pair.first;
            int package_seq_num = pair.second;
            delete connection;

            boost::asio::ip::udp::endpoint receiver_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(connection->get_tcp_socket()->remote_endpoint().address().to_string()), connection->get_remote_udp_port());

            connection = connection_factory->get_connection(connection_id);
            std::fstream *file = connection->get_file();
            int package_size = connection->get_package_size();
            int offset = package_seq_num * package_size;
            std::vector<char> bytes = file_manager->get_file_bytes(file, offset, package_size);
            fup::core::entity::header h;
            h.package_seq_num = package_seq_num;
            h.checksum = checksum_service->create_checksum(bytes);
            h.body_length = bytes.size();
            fup::core::entity::package p(h, bytes);
            connection->get_sender_service()->send_package(p, receiver_endpoint);
            file_manager->close_file(file);
        }

        void client::download_file(std::string address, std::string file_name, int package_size)
        {
            fup::core::connection *connection = connect(address, file_name, true, package_size);
            initiate_handshake(connection, file_name, true, package_size);

            std::fstream *file = file_manager->open_file(file_name, true);
            connection->set_file(file, file_name);

            fup::facade::manager::upload_download_manager::download_file(connection, checksum_service, file);

            connection_factory->delete_connection(connection->get_id());
        }

        void client::upload_file(std::string address, std::string path_to_file, int package_size)
        {
            std::fstream *file = file_manager->open_file(path_to_file);
            fup::core::connection *connection = connect(address, path_to_file, false, package_size);
            initiate_handshake(connection, path_to_file, false, package_size);

            fup::core::entity::metadata send_op_metadata = file_manager->get_metadata(file, connection->get_package_size(), connection->get_file_name());
            connection->get_sender_service()->send_metadata(send_op_metadata);
            fup::facade::manager::upload_download_manager::upload_file(connection, checksum_service, file_manager, file);

            file_manager->close_file(file);
            connection_factory->delete_connection(connection->get_id());
        }
    }
}
