
#include "client.hpp"

namespace fup
{
    namespace facade
    {
        fup::core::connection *client::connect_to_remote(std::string address, std::string file_name, bool is_download, int packet_size)
        {
            // Deduce address and port number
            std::string port("80");
            int index = address.find(":");
            if (index != -1)
            {
                port = address.substr(index + 1);
                address = address.substr(0, index);
            }

            // Get address information from the given url port
            addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_protocol = SOCK_STREAM;

            int err = getaddrinfo(address.c_str(), port.c_str(), &hints, &res);
            if (err != 0)
            {
                throw std::runtime_error("Cant fetch the ip of the given address and port");
            }
            // Cast it to sockaddr_in to add the port
            sockaddr_in remote_tcp = *(sockaddr_in *)(res->ai_addr);
            remote_tcp.sin_port = ntohs(stoi(port));

            // Connect with tcp
            fup::core::connection *connection = connection_factory->get_connection(tcp_socket, udp_socket);
            connection->set_remote_address(address);
            connect(connection->get_tcp_socket(), (sockaddr *)&remote_tcp, sizeof(remote_tcp));
            return connection;
        }

        void client::initiate_handshake(fup::core::connection *connection, std::string file_name, bool is_download, int packet_size)
        {
            // Prepare a request to server
            fup::core::entity::request req;
            req.connection_id = connection->get_id();
            req.file_name = file_name;
            req.is_download = is_download;
            req.packet_size = packet_size;

            // Send request and await for response
            connection->get_sender_service()->send_request(req);
            fup::core::entity::response resp = connection->get_receiver_service()->receive_response();
            if (resp.status < 0)
            {
                std::string error_msg = std::string("Response status: ") + std::to_string(resp.status);
                throw new std::runtime_error(error_msg);
            }
            connection->set_remote_connection_id(resp.connection_id);
            connection->set_remote_udp_port(resp.udp_port);

            // Get address information with address and udp port obtained from response
            addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_protocol = SOCK_DGRAM;

            int err = getaddrinfo(connection->get_remote_address().c_str(), std::to_string(connection->get_remote_udp_port()).c_str(), &hints, &res);
            if (err != 0)
            {
                throw std::runtime_error("Cant fetch the ip of the given address and port");
            }
            // Udp connect to server udp
            connect(connection->get_udp_socket(), res->ai_addr, res->ai_addrlen);
        }

        void client::handle_resend(fup::core::connection *connection)
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

        void client::download_file(std::string address, std::string file_name, int packet_size)
        {
            fup::core::connection *connection = connect_to_remote(address, file_name, true, packet_size);
            initiate_handshake(connection, file_name, true, packet_size);

            std::fstream *file = file_manager->open_file(file_name, true);
            connection->set_file(file, file_name);

            fup::facade::manager::upload_download_manager::download_file(connection, checksum_service, file);

            connection_factory->delete_connection(connection->get_id());
        }

        void client::upload_file(std::string address, std::string path_to_file, int packet_size)
        {
            std::fstream *file = file_manager->open_file(path_to_file);
            fup::core::connection *connection = connect_to_remote(address, path_to_file, false, packet_size);
            initiate_handshake(connection, path_to_file, false, packet_size);

            fup::core::entity::metadata send_op_metadata = file_manager->get_metadata(file, connection->get_packet_size(), connection->get_file_name());
            connection->get_sender_service()->send_metadata(send_op_metadata);
            fup::facade::manager::upload_download_manager::upload_file(connection, checksum_service, file_manager, file);

            file_manager->close_file(file);
            connection_factory->delete_connection(connection->get_id());
        }

        client::client(std::string file_location, manager::file_manager *file_manager_injected, fup::core::interface::checksum *checksum_injected)
        {
            tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
            udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

            // TODO in the future make this optional with different other checksum_services possible
            checksum_service = (checksum_injected == nullptr) ? new fup::core::service::blake3_checksum() : checksum_injected;
            file_manager = (file_manager_injected == nullptr) ? new fup::facade::manager::file_manager(file_location) : file_manager_injected;

            connection_factory = new fup::facade::helper::connection_factory();
        }

        client::~client()
        {
            delete checksum_service;
            delete file_manager;
            delete connection_factory;
            close(udp_socket);
            close(tcp_socket);
        }
    }
}
