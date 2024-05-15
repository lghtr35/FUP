#pragma once
#ifndef FUP_UP_DOWN_MANAGER_HPP
#define FUP_UP_DOWN_MANAGER_HPP
#include "core/core.hpp"
#include "core/connection.hpp"
#include "file_manager.hpp"

namespace fup
{
    namespace facade
    {
        namespace manager
        {
            class upload_download_manager
            {
            public:
                static void download_file(fup::core::connection *&connection, fup::core::interface::checksum *&checksum_service, std::fstream *&file);
                static void upload_file(fup::core::connection *&connection, fup::core::interface::checksum *&checksum_service, file_manager *&file_manager, std::fstream *&file);
            };
        }
    }
}

#endif