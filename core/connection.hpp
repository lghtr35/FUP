#pragma once
#ifndef FUP_CORE_CONNECTION_HPP
#define FUP_CORE_CONNECTION_HPP

#include "service/sender.hpp"
#include "service/receiver.hpp"
#include "helper/helper.hpp"
#include <fstream>

namespace fup
{
    namespace core
    {
        class connection
        {
        public:
            connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp, boost::shared_ptr<interface::checksum> checksum, unsigned int idx);
            ~connection();
            service::sender *get_sender_service();
            service::receiver *get_receiver_service();
            boost::asio::ip::tcp::socket *get_tcp_socket();
            boost::asio::ip::udp::socket *get_udp_socket();
            std::streampos get_file_begin();
            std::ifstream *get_file();
            std::streampos get_file_end();
            std::string get_file_name();
            unsigned int get_id();
            unsigned int get_packet_size();
            void set_packet_size(unsigned int ps);
            void set_file(std::ifstream *file_to_use, std::string file_name_to_use);

        private:
            service::receiver *receiver_service;
            service::sender *sender_service;
            boost::asio::ip::tcp::socket *tcp_socket;
            boost::asio::ip::udp::socket *udp_socket;
            std::string file_name;
            std::streampos file_begin;
            std::ifstream *file;
            std::streampos file_end;
            unsigned int id;
            unsigned int packet_size;
        };
    }
}

#endif