#pragma once

#ifndef FUP_FACADE_HELPER_CONN_FACTORY_HPP
#define FUP_FACADE_HELPER_CONN_FACTORY_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include "core/connection.hpp"
#include <mutex>

namespace fup
{
    namespace facade
    {
        namespace helper
        {
            class connection_factory
            {
            public:
                connection_factory();

                ~connection_factory();

                fup::core::connection *get_connection(int tcp, int udp);

                fup::core::connection *get_connection(int id);

                unsigned int get_connection_count();

                void delete_connection(int id);

            private:
                std::mutex mutex;
                std::vector<fup::core::connection *> connections;
                unsigned int connection_count;
            };
        }
    }
}

#endif