#pragma once
#include "server.hpp"

namespace fup
{
    namespace facade
    {
        void server::listen()
        {
            fup::core::connection *connection = connection_factory->get_connection(&socket_factory->get_tcp(), &socket_factory->get_udp());
            acceptor->async_accept(*connection->get_tcp_socket(), std::bind(&fup::facade::server::handle_accept, this, connection, boost::asio::placeholders::error));
        }

        void server::handle_accept(fup::core::connection *connection, const boost::system::error_code &error)
        {
            if (!error)
            {
                std::cout << "Accepted connection on: "
                          << connection->get_tcp_socket()->remote_endpoint().address().to_string()
                          << ":" << connection->get_tcp_socket()->remote_endpoint().port() << '\n';
                connection->get_sender_service()->send_ok();
                try
                {
                    std::string message_identifier = connection->get_receiver_service()->receive_message_identifier();
                    if (message_identifier == "RE")
                    {
                        handle_resend(connection);
                        delete connection;
                    }
                    else if (message_identifier == "SE")
                    {
                        handle_handshake(connection);
                    }
                }
                catch (std::exception exception)
                {
                    // TODO think what to do in this scenario
                }
            }
            listen();
        }

        void server::handle_resend(fup::core::connection *connection)
        {
        }

        void server::handle_handshake(fup::core::connection *connection)
        {
            fup::core::entity::request *request = connection->get_receiver_service()->receive_request();
            std::ifstream *file = file_manager->open_file(request->file_name);
            connection->set_file(file, request->file_name);
            connection->set_packet_size(request->packet_size);
            if (request->is_download)
            {
                fup::core::entity::metadata send_op_metadata = file_manager->get_metadata(file, request->packet_size, request->file_name);
                connection->get_sender_service()->send_metadata(send_op_metadata);
                // We call server upload file because client wants to download a file so server should upload to the client
                upload_file(connection, file);
            }
            else
            {
                // TODO implement upload file functionality
                // Should be pretty much client side download op
                download_file(connection, file);
            }
        }

        void server::upload_file(fup::core::connection *connection, std::ifstream *file)
        {
            // Listening UDP port always going to be on tcp-1
            boost::asio::ip::udp::endpoint receiver_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(connection->get_tcp_socket()->remote_endpoint().address().to_string()), connection->get_tcp_socket()->remote_endpoint().port() - 1);
            // Get a random available udp socket
            connection->get_udp_socket()->open(boost::asio::ip::udp::v4());
            // Read file while creating packets and send them to client
            fup::core::entity::header h;
            int seq_num = 0;
            int packet_size = connection->get_packet_size();
            int file_total_size = file_manager->get_file_size(file, connection->get_file_name());
            int offset = 0;
            while (offset <= file_total_size)
            {
                offset = seq_num * packet_size;
                std::vector<char> bytes = file_manager->get_file_bytes(file, offset, packet_size);
                h.body_length = bytes.size();
                h.packet_seq_num = seq_num;
                h.checksum = checksum_service->create_checksum(bytes);
                fup::core::entity::packet p(h, bytes);
                connection->get_sender_service()->send_packet(p, receiver_endpoint);
                seq_num++;
            }
        }
    }
}