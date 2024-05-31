
#include "server.hpp"

namespace fup
{
    namespace facade
    {
        void server::run(int thread_count)
        {
            threads.reserve(thread_count - 1);

            if (listen(tcp_socket, SOMAXCONN) != 0)
            {
                throw std::runtime_error("Server not able to listen");
            }

            sockaddr_in client;
            while (true)
            {
                socklen_t clilen = sizeof(client);
                int handler = accept(tcp_socket, (sockaddr *)&client, &clilen);

                // Block program loop until we find a thread that we can run the new connection on
                bool found;
                while (found == false)
                {
                    // Try to fetch a thread and run the new connection on
                    for (std::size_t i = 0; i < thread_count - 1; ++i)
                    {
                        if (!threads[i].joinable())
                        {
                            threads[i] = std::thread(&fup::facade::server::do_accept, this, handler, client);
                            found = true;
                            break;
                        }
                    }

                    // Join all threads in the pool to make room for new connections.
                    for (std::size_t i = 0; i < threads.size(); ++i)
                    {
                        if (threads[i].joinable())
                        {
                            threads[i].join();
                        }
                    }
                }
            }
        }

        void server::do_accept(int handler, sockaddr_in client)
        {
            int udp = socket(AF_INET, SOCK_DGRAM, 0);
            sockaddr_in server;
            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            server.sin_family = AF_INET;
            server.sin_port = ntohs(0);
            if (bind(udp, (sockaddr *)&server, sizeof(server)) != 0)
            {
                throw std::runtime_error("Cant set udp port for binding");
            }

            fup::core::connection *connection = connection_factory->get_connection(handler, udp);
            connection->set_remote_address(inet_ntoa(client.sin_addr));
            connection->set_remote_tcp_port(client.sin_port);
            connection->set_udp_port(server.sin_port);

            handle_accept(connection);
            close(udp);
            delete connection;
        }

        void server::handle_accept(fup::core::connection *connection)
        {
            std::cout << "Accepted connection on: "
                      << connection->get_remote_address()
                      << ":" << connection->get_remote_tcp_port() << '\n';

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
                connection_factory->delete_connection(connection->get_id());
                std::cout << "An error occured: " << exception.what() << "\n";
            }
        }

        void server::handle_resend(fup::core::connection *connection)
        {
            std::pair<int, int> pair = connection->get_receiver_service()->receive_resend();
            int connection_id = pair.first;
            int packet_seq_num = pair.second;
            delete connection;

            connection = connection_factory->get_connection(connection_id);
            std::fstream *file = connection->get_file();
            int packet_size = connection->get_packet_size();
            int offset = packet_seq_num * packet_size;
            std::vector<char> bytes = file_manager->get_file_bytes(file, offset, packet_size);
            fup::core::entity::header h;
            h.packet_seq_num = packet_seq_num;
            h.checksum = checksum_service->create_checksum(bytes);
            h.body_length = bytes.size();
            fup::core::entity::packet p(h, bytes);
            connection->get_sender_service()->send_packet(p);
            file_manager->close_file(file);
        }

        void server::handle_handshake(fup::core::connection *connection)
        {
            fup::core::entity::request request = connection->get_receiver_service()->receive_request();
            connection->set_packet_size(request.packet_size);
            connection->set_remote_connection_id(request.connection_id);

            fup::core::entity::response response;
            response.status = 0;
            response.connection_id = connection->get_id();
            response.udp_port = connection->get_udp_port();
            connection->get_sender_service()->send_response(response);

            if (listen(connection->get_udp_socket(), 4) != 0)
            {
                throw std::runtime_error(std::string("Cant listen with udp socket on connection_id: ") + std::to_string(connection->get_id()));
            }

            if (request.is_download)
            {
                std::fstream *file = file_manager->open_file(request.file_name);
                connection->set_file(file, request.file_name);
                // We call server upload file because client wants to download a file so server should upload to the client
                upload_file(connection, file);
            }
            else
            {
                std::fstream *file = file_manager->open_file(request.file_name, true);
                connection->set_file(file, request.file_name);
                download_file(connection, file);
            }
        }

        void server::upload_file(fup::core::connection *connection, std::fstream *file)
        {
            fup::core::entity::metadata send_op_metadata = file_manager->get_metadata(file, connection->get_packet_size(), connection->get_file_name());
            connection->get_sender_service()->send_metadata(send_op_metadata);
            fup::facade::manager::upload_download_manager::upload_file(connection, checksum_service, file_manager, file);

            connection_factory->delete_connection(connection->get_id());
            file_manager->close_file(file);
        }

        void server::download_file(fup::core::connection *connection, std::fstream *file)
        {
            fup::facade::manager::upload_download_manager::download_file(connection, checksum_service, file);
            connection_factory->delete_connection(connection->get_id());
            file_manager->close_file(file);
        }

        server::server(int port, manager::file_manager *file_manager_injected, fup::core::interface::checksum *checksum_injected)
        {
            // TODO in the future make this optional with different other checksum_services possible
            checksum_service = (checksum_injected == nullptr) ? new fup::core::service::blake3_checksum() : checksum_injected;
            file_manager = (file_manager_injected == nullptr) ? new fup::facade::manager::file_manager("/") : file_manager_injected;

            connection_factory = new fup::facade::helper::connection_factory();

            tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in server;
            server.sin_family = AF_INET;
            server.sin_port = port;
            server.sin_addr.s_addr = inet_addr("127.0.0.1");
            if (bind(tcp_socket, (sockaddr *)&server, sizeof(server)) != 0)
            {
                throw std::runtime_error("Cant configure TCP socket on this port");
            }
        }

        server::~server()
        {
            delete checksum_service;
            delete file_manager;
            delete connection_factory;
            close(tcp_socket);
        }
    }
}