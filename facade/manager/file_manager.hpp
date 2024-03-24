#pragma once
#ifndef FUP_FILE_MANAGER_HPP
#define FUP_FILE_MANAGER_HPP
#include <core/core.hpp>
#include "../helper/helper.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            class file_manager
            {
            public:
                std::ifstream *open_file(std::string file_name);
                fup::core::entity::metadata get_metadata(std::ifstream *fs, unsigned int file_packet_size, std::string file_name);
                std::vector<char> get_file_bytes(std::ifstream *fs, unsigned int offset, unsigned int size);
                void close_file(std::ifstream *fs);
                file_manager(std::string fl);
                ~file_manager();

            private:
                std::string get_file_extension(std::string file_name);
                std::filesystem::path files_location;
            };
        }
    }
}

#endif