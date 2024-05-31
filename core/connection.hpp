#pragma once
#ifndef FUP_CORE_CONNECTION_HPP
#define FUP_CORE_CONNECTION_HPP

#include "service/sender.hpp"
#include "service/receiver.hpp"
#include <fstream>

namespace fup
{
    namespace core
    {
        class connection
        {
        public:
            // Constructors
            connection(int tcp, int udp, unsigned int idx);
            ~connection();
            // Getters
            service::sender *get_sender_service();
            service::receiver *get_receiver_service();
            int get_tcp_socket();
            int get_udp_socket();
            std::streampos get_file_begin();
            std::fstream *get_file();
            std::streampos get_file_end();
            std::string get_file_name();
            unsigned int get_id();
            unsigned int get_packet_size();
            unsigned int get_remote_udp_port();
            unsigned int get_remote_tcp_port();
            unsigned int get_remote_connection_id();
            unsigned int get_udp_port();
            std::string get_remote_address();
            // Setters
            void set_packet_size(unsigned int ps);
            void set_file(std::fstream *file_to_use, std::string file_name_to_use);
            void set_remote_udp_port(unsigned int port);
            void set_remote_tcp_port(unsigned int port);
            void set_remote_connection_id(unsigned int id);
            void set_remote_address(std::string address);
            void set_udp_port(unsigned int port);

        private:
            service::receiver *receiver_service;
            service::sender *sender_service;
            int tcp_socket;
            int udp_socket;
            std::string file_name;
            std::streampos file_begin;
            std::fstream *file;
            std::streampos file_end;
            unsigned int id;
            unsigned int packet_size;
            unsigned int remote_udp_port;
            unsigned int remote_tcp_port;
            unsigned int remote_connection_id;
            std::string remote_address;
            unsigned int udp_port;
        };
    }
}

#endif