#pragma once
#ifndef FUP_COMBINED_MANAGER_HPP
#define FUP_COMBINED_MANAGER_HPP
#include <core/core.hpp>

namespace fup
{
    namespace facade
    {
        class combined_manager
        {
        public:
            // Server
            void listen();
            void handle_handshake();
            void send_file();
            // Client
            void init_handshake();
            void handle_file();
            // House Keeping
            combined_manager();
            ~combined_manager();

        private:
            fup::core::sender sender;
            fup::core::receiver receiver;
            fup::core::helper::socket_singleton_factory socket_factory;
        };
    }
}

#endif