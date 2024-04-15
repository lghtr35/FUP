#pragma once

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
                socket_factory(boost::asio::io_context &ctx)
                {
                    io_context = &ctx;
                }

                ~socket_factory()
                {
                    // io_context memory should be managed on parent class
                }

                boost::asio::ip::udp::socket get_udp()
                {
                    return boost::asio::ip::udp::socket(*io_context);
                }

                boost::asio::ip::udp::socket get_udp(int port)
                {
                    return boost::asio::ip::udp::socket(*io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
                }

                boost::asio::ip::tcp::socket get_tcp()
                {
                    return boost::asio::ip::tcp::socket(*io_context);
                }

                boost::asio::io_context *io_context;
            };
        }
    }
}
