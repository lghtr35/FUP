#pragma once
#ifndef FUP_UP_DOWN_MANAGER_HPP
#define FUP_UP_DOWN_MANAGER_HPP
#include "core/core.hpp"
#include "core/connection.hpp"
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
                    fup::core::entity::metadata metadata = connection->get_receiver_service()->receive_metadata();
                    size_t offset = 0;
                    size_t received_bytes = 0;
                    unsigned int packet_size = connection->get_packet_size();
                    while (received_bytes <= metadata.file_total_size)
                    {
                        fup::core::entity::packet p = connection->get_receiver_service()->receive_packet();
                        if (!checksum_service->validate_checksum(p.body, p.header.checksum) || p.header.body_length != p.body.size())
                        {
                            connection->get_sender_service()->send_resend(connection->get_remote_connection_id(), p.header.packet_seq_num);
                            offset = 0;
                            continue;
                        }
                        offset = packet_size * p.header.packet_seq_num;
                        file->seekp(offset);
                        file->write(p.body.data(), p.header.body_length);
                        received_bytes += p.header.body_length;
                    }
                }

                static void upload_file(fup::core::connection *&connection, fup::core::interface::checksum *&checksum_service, file_manager *&file_manager, std::fstream *&file)
                {
                    // Read file while creating packets and send them to client
                    unsigned int seq_num = 0;
                    unsigned int packet_size = connection->get_packet_size();
                    unsigned int file_total_size = file_manager->get_file_size(file, connection->get_file_name());
                    size_t offset = 0;
                    while (offset <= file_total_size)
                    {
                        fup::core::entity::header h;
                        offset = seq_num * packet_size;
                        std::vector<char> bytes = file_manager->get_file_bytes(file, offset, packet_size);
                        h.body_length = bytes.size();
                        h.packet_seq_num = seq_num;
                        h.checksum = checksum_service->create_checksum(bytes);
                        fup::core::entity::packet p(h, bytes);
                        connection->get_sender_service()->send_packet(p);
                        seq_num++;
                    }
                }
            };
        }
    }
}

#endif