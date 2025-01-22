#ifndef FUP_CLIENT_HPP
#define FUP_CLIENT_HPP

#include "common.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#ifndef FUP_DEFAULT_MAX_THREAD
#define FUP_DEFAULT_MAX_THREAD 16
#endif

namespace fup
{
    /*
     * Can download one file at a time
     * Might need refactoring to allow more versions of FUP to be implemented as submodules
     */

    class client
    {
    public:
        client(unsigned int version, std::string save_location);
        client(unsigned int version, std::string save_location, size_t max_threads);
        ~client();

        int upload(std::string destination_url, std::string destination_port, std::string file_fullpath);
        int download(std::string destination_url, std::string destination_port, std::string filename);
        int list_files(std::string destination_url, std::string destination_port, std::string *res);

    private: 
        version version;
        std::string save_location;
        size_t max_thread_limit;
        int _init_socket(std::string destination_url, std::string destination_port, int sock_type);
        void _connect(std::string destination_url, std::string destination_port_tcp, std::string destination_port_udp);
        void _receive_and_write_packet();
        void _disconnect();
    };
}

#endif