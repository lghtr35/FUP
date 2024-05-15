
#include "connection.hpp"

namespace fup
{
    namespace core
    {
        connection::connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, unsigned int idx)
        {
            receiver_service = new fup::core::service::receiver(tcp, udp);
            sender_service = new fup::core::service::sender(tcp, udp);
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

        unsigned int connection::get_package_size()
        {
            return package_size;
        }

        unsigned int connection::get_id()
        {
            return id;
        }

        unsigned int connection::get_remote_udp_port()
        {
            return remote_udp_port;
        }

        unsigned int connection::get_remote_connection_id()
        {
            return remote_connection_id;
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

        void connection::set_package_size(unsigned int ps)
        {
            package_size = ps;
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

        void connection::set_remote_connection_id(unsigned int id)
        {
            remote_connection_id = id;
        }
    }
}