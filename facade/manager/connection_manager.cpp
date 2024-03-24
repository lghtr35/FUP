#pragma once
#include "connection_manager.hpp"

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            void connection_manager::listen()
            {
                while (connection_factory->get_connection_count() > max_connections)
                {
                }
                fup::core::connection *connection = connection_factory->get_connection();
                acceptor->async_accept(*connection->get_tcp_socket(), std::bind(&fup::facade::manager::connection_manager::handle_accept, this, connection->get_id(), boost::asio::placeholders::error));
            }

            void connection_manager::handle_accept(unsigned int id, const boost::system::error_code &error)
            {
                fup::core::connection *connection = connection_factory->get_connection(id);

                if (!error)
                {
                    connection->get_sender_service()->send_ok();
                    try
                    {
                        handle_handshake(connection);
                    }
                    catch (std::exception exception)
                    {
                        // TODO think what to do in this scenario
                    }
                }
                else
                {
                }
            }

            void connection_manager::handle_handshake(fup::core::connection *connection)
            {
                fup::core::entity::request *request = connection->get_receiver_service()->receive_request();
                // TODO obtain file metadata and udp connection info and send metadata
                if (request->is_download)
                {
                    std::ifstream *file = file_manager->open_file(request->file_name);
                    connection->set_file(file);
                    connection->set_packet_size(request->packet_size);
                    fup::core::entity::metadata send_op_metadata = file_manager->get_metadata(file, request->packet_size, request->file_name);
                    connection->get_sender_service()->send_metadata(send_op_metadata);
                    // TODO Implement this function
                    // We call server upload file because client wants to download a file so server should upload to the client
                    server_upload_file();
                }
                else
                {
                }

                connection_factory->delete_connection(connection->get_id());
            }
        }
    }
}