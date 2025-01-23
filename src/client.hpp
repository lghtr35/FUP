#ifndef FUP_CLIENT_HPP
#define FUP_CLIENT_HPP

#include "Common.hpp"
#include "Connection.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fup
{
    /*
     * Can download one file at a time
     * Might need refactoring to allow more versions of FUP to be implemented as submodules
     */

    class Client
    {
    public:
        Client(unsigned int version, std::string save_location);
        Client(unsigned int version, std::string save_location, size_t max_threads);
        ~Client();

        int Upload(std::string destinationUrl, std::string destinationPort, std::string fileFullpath);
        int Download(std::string destinationUrl, std::string destinationPort, std::string filename);
        int ListFiles(std::string destinationUrl, std::string destinationPort, std::string *res);

    private: 
        Version version;
        std::string saveLocation;
        size_t threadLimit;
        std::unique_ptr<Connection> conn;
        int initTcp(std::string destinationUrl, std::string destinationPort);
        void connectToDestination(std::string destinationUrl, std::string destinationPort_tcp);
        void disconnect();
    };
}

#endif