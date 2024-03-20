#pragma once
#include <facade/connection_manager.hpp>

namespace fup
{
    namespace facade
    {
        void connection_manager::listen()
        {
            if (connection_factory->get_connection_count() <= max_connections)
            {
            }
        }
    }
}