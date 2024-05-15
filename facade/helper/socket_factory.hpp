#pragma once

#ifndef FUP_FACADE_HELPER_SOCK_FACTORY_HPP
#define FUP_FACADE_HELPER_SOCK_FACTORY_HPP
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/asio.hpp>

namespace fup
{
    namespace facade
    {
        namespace helper
        {
            class socket_factory
            {
            public:
                socket_factory(boost::asio::io_context &ctx);
                ~socket_factory();
                boost::asio::ip::udp::socket *get_udp();
                boost::asio::ip::udp::socket *get_udp(int port);
                boost::asio::ip::tcp::socket *get_tcp();

                boost::asio::io_context *io_context;
            };
        }
    }
}
#endif