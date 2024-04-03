#pragma once
#include "connection.hpp"

namespace fup
{
    namespace core
    {
        connection::connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<core::interface::checksum> checksum, unsigned int idx)
        {
            receiver_service = new fup::core::service::receiver(tcp, udp, checksum);
            sender_service = new fup::core::service::sender(tcp, udp, checksum);
            tcp_socket = tcp;
            udp_socket = udp;
            id = idx;
        }

        connection::~connection()
        {
            delete receiver_service;
            delete sender_service;
            delete tcp_socket;
            delete udp_socket;
        }

        boost::asio::ip::tcp::socket *connection::get_tcp_socket()
        {
            return tcp_socket;
        }

        fup::core::service::receiver *connection::get_receiver_service()
        {
            return receiver_service;
        }

        fup::core::service::sender *connection::get_sender_service()
        {
            return sender_service;
        }

        unsigned int connection::get_packet_size()
        {
            return packet_size;
        }

        unsigned int connection::get_id()
        {
            return id;
        }

        std::streampos connection::get_file_begin()
        {
            return file_begin;
        }

        std::streampos connection::get_file_end()
        {
            return file_end;
        }

        std::ifstream *connection::get_file()
        {
            return file;
        }

        std::string connection::get_file_name()
        {
            return file_name;
        }

        void connection::set_packet_size(unsigned int ps)
        {
            packet_size = ps;
        }

        void connection::set_file(std::ifstream *file_to_use, std::string file_name_to_use)
        {
            file_name = file_name_to_use;
            file = file_to_use;
            file_begin = file_to_use->tellg();
            file_to_use->seekg(0, std::ios::end);
            file_end = file_to_use->tellg();
            file_to_use->seekg(0, std::ios::beg);
        }
    }
}