
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

            std::fstream *file_manager::open_file(std::string file_name, bool is_write)
            {
                std::fstream *file = new std::fstream();
                std::ios::openmode open_mode = is_write ? std::ios::out : std::ios::in;
                file->open(files_location / std::filesystem::path(file_name), open_mode | std::ios::binary);

                return file;
            }

            void file_manager::close_file(std::fstream *fs)
            {
                fs->close();
                delete fs;
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
            fup::core::entity::metadata file_manager::get_metadata(std::fstream *fs, unsigned int file_package_size, std::string file_name)
            {
                return fup::core::entity::metadata(file_package_size,
                                                   get_file_size(fs, file_name),
                                                   file_name,
                                                   get_file_extension(file_name));
            }

            std::vector<char> file_manager::get_file_bytes(std::fstream *fs, unsigned int offset, unsigned int size)
            {
                std::vector<char> bytes(size);
                fs->seekg(offset, std::ios::beg);
                fs->readsome(bytes.data(), size);

                return bytes;
            }

            size_t file_manager::get_file_size(std::fstream *fs, std::string file_name)
            {
                return std::filesystem::file_size(files_location / std::filesystem::path(file_name));
            }
        }
    }
}