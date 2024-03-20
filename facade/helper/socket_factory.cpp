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
                socket_factory(boost::shared_ptr<boost::asio::io_context> ctx)
                {
                    io_context = ctx;
                }

                ~socket_factory()
                {
                    // Dont delete io_context cause it is shared
                }

                boost::asio::ip::udp::socket *get_udp()
                {
                    return new boost::asio::ip::udp::socket(*io_context);
                }

                boost::asio::ip::tcp::socket *get_tcp()
                {
                    return new boost::asio::ip::tcp::socket(*io_context);
                }

            private:
                boost::shared_ptr<boost::asio::io_context> io_context;
            };
        }
    }
}
