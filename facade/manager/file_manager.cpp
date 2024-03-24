#pragma once
#include "file_manager.hpp"

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            file_manager::file_manager(std::string fl)
            {
                files_location = fl;
            }

            file_manager::~file_manager() {}

            std::ifstream *file_manager::open_file(std::string file_name)
            {
                std::ifstream file;
                file.open(files_location / std::filesystem::path(file_name), std::ios::in | std::ios::binary);

                return &file;
            }

            void file_manager::close_file(std::ifstream *fs)
            {
                fs->close();
            }

            std::string file_manager::get_file_extension(std::string file_name)
            {
                size_t index;
                while ((index = file_name.find(".")) != std::string::npos)
                {
                    file_name.erase(0, index + 1);
                }
                return file_name;
            }
            fup::core::entity::metadata file_manager::get_metadata(std::ifstream *fs, unsigned int file_packet_size, std::string file_name)
            {
                return fup::core::entity::metadata(file_packet_size,
                                                   std::filesystem::file_size(files_location / std::filesystem::path(file_name)),
                                                   file_name,
                                                   get_file_extension(file_name));
            }

            // TODO: think about this
            std::vector<char> file_manager::get_file_bytes(std::ifstream *fs, unsigned int offset, unsigned int size)
            {
                std::vector<char> bytes(size);
                fs->seekg(offset, std::ios::beg);
                fs->readsome(bytes.data(), size);

                return bytes;
            }
        }
    }
}