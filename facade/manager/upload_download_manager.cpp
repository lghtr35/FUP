#pragma once
#include <core/core.hpp>
#include "file_manager.hpp"

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            class upload_download_manager
            {
            public:
                static void download_file(fup::core::connection *&connection, fup::core::interface::checksum *&checksum_service, std::fstream *&file)
                {
                    // TODO Add queue so that dont lose packages
                    fup::core::entity::metadata *metadata = connection->get_receiver_service()->receive_metadata();
                    size_t offset = 0;
                    size_t received_bytes = 0;
                    unsigned int package_size = connection->get_package_size();
                    while (received_bytes <= metadata->file_total_size)
                    {
                        fup::core::entity::package p = *connection->get_receiver_service()->receive_package();
                        if (!checksum_service->validate_checksum(p.body, p.header.checksum) || p.header.body_length != p.body.size())
                        {
                            connection->get_sender_service()->send_resend(connection->get_remote_connection_id(), p.header.package_seq_num);
                            offset = 0;
                            continue;
                        }
                        offset = package_size * p.header.package_seq_num;
                        file->seekp(offset);
                        file->write(p.body.data(), p.body.size());
                        received_bytes += p.header.body_length;
                    }

                    delete metadata;
                }

                static void upload_file(fup::core::connection *&connection, fup::core::interface::checksum *&checksum_service, file_manager *&file_manager, std::fstream *&file)
                {
                    boost::asio::ip::udp::endpoint receiver_endpoint = boost::asio::ip::udp::endpoint(connection->get_tcp_socket()->remote_endpoint().address(), connection->get_remote_udp_port());
                    // Get a random available udp socket
                    connection->get_udp_socket()->open(boost::asio::ip::udp::v4());
                    // Read file while creating packages and send them to client
                    unsigned int seq_num = 0;
                    unsigned int package_size = connection->get_package_size();
                    unsigned int file_total_size = file_manager->get_file_size(file, connection->get_file_name());
                    size_t offset = 0;
                    while (offset <= file_total_size)
                    {
                        fup::core::entity::header h;
                        offset = seq_num * package_size;
                        std::vector<char> bytes = file_manager->get_file_bytes(file, offset, package_size);
                        h.body_length = bytes.size();
                        h.package_seq_num = seq_num;
                        h.checksum = checksum_service->create_checksum(bytes);
                        fup::core::entity::package p(h, bytes);
                        connection->get_sender_service()->send_package(p, receiver_endpoint);
                        seq_num++;
                    }
                }
            };
        }
    }
}