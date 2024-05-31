
#include "connection.hpp"

namespace fup
{
    namespace core
    {
        connection::connection(int tcp, int udp, unsigned int idx)
        {

            tcp_socket = tcp;
            udp_socket = udp;
            receiver_service = new fup::core::service::receiver(tcp_socket, udp_socket);
            sender_service = new fup::core::service::sender(tcp_socket, udp_socket);
            id = idx;
        }

        connection::~connection()
        {
            delete receiver_service;
            delete sender_service;
            close(tcp_socket);
            close(udp_port);
        }

        int connection::get_udp_socket()
        {
            return udp_socket;
        }

        int connection::get_tcp_socket()
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

        unsigned int connection::get_remote_udp_port()
        {
            return remote_udp_port;
        }

        unsigned int connection::get_remote_tcp_port()
        {
            return remote_tcp_port;
        }

        unsigned int connection::get_remote_connection_id()
        {
            return remote_connection_id;
        }

        unsigned int connection::get_udp_port()
        {
            return udp_port;
        }

        std::streampos connection::get_file_begin()
        {
            return file_begin;
        }

        std::streampos connection::get_file_end()
        {
            return file_end;
        }

        std::fstream *connection::get_file()
        {
            return file;
        }

        std::string connection::get_file_name()
        {
            return file_name;
        }

        std::string connection::get_remote_address()
        {
            return remote_address;
        }

        void connection::set_packet_size(unsigned int ps)
        {
            packet_size = ps;
        }

        void connection::set_file(std::fstream *file_to_use, std::string file_name_to_use)
        {
            file_name = file_name_to_use;
            file = file_to_use;
            file_begin = file_to_use->tellg();
            file_to_use->seekg(0, std::ios::end);
            file_end = file_to_use->tellg();
            file_to_use->seekg(0, std::ios::beg);
        }

        void connection::set_remote_udp_port(unsigned int port)
        {
            remote_udp_port = port;
        }

        void connection::set_remote_tcp_port(unsigned int port)
        {
            remote_tcp_port = port;
        }

        void connection::set_remote_connection_id(unsigned int id)
        {
            remote_connection_id = id;
        }

        void connection::set_remote_address(std::string address)
        {
            remote_address = address;
        }

        void connection::set_udp_port(unsigned int port)
        {
            udp_port = port;
        }
    }
}