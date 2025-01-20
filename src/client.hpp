#ifndef FUP_CLIENT_H
#define FUP_CLIENT_H

#include "common.hpp"
#include "connection.hpp"
#include <thread>

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
        ~client();

        int upload(std::string destination_url, std::string destination_port, std::string file_fullpath);
        int download(std::string destination_url, std::string destination_port, std::string filename);
        int list_files(std::string destination_url, std::string destination_port, std::string *res);

    private: 
        version version;
        std::string save_location;
        std::unique_ptr<connection> conn;
        int _init_socket(std::string destination_url, std::string destination_port, int sock_type);
        void _connect(std::string destination_url, std::string destination_port);
        void _disconnect();
    };
}

#endif