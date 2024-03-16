#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/asio.hpp>

namespace fup
{
    namespace core
    {
        namespace helper
        {
            class socket_singleton_factory
            {
            public:
                socket_singleton_factory()
                {
                    io_context = nullptr;
                    udp = nullptr;
                    tcp = nullptr;
                }

                ~socket_singleton_factory()
                {
                    delete io_context;
                    delete udp;
                    delete tcp;
                }

                void set_io_context(boost::asio::io_context *ctx)
                {
                    // TODO: think about what to do here
                    if (ctx == nullptr)
                    {
                        set_io_context();
                        return;
                    }
                    io_context = ctx;
                }

                void set_io_context()
                {
                    if (io_context == nullptr)
                    {
                        io_context = new boost::asio::io_context();
                    }
                }

                boost::asio::io_context *get_io_context()
                {
                    if (io_context == nullptr)
                    {
                        set_io_context();
                    }
                    return io_context;
                }

                boost::asio::ip::udp::socket *get_udp()
                {
                    if (udp == nullptr)
                    {
                        set_io_context();
                        udp = new boost::asio::ip::udp::socket(*io_context);
                    }

                    return udp;
                }

                boost::asio::ip::tcp::socket *get_tcp()
                {

                    if (tcp == nullptr)
                    {
                        set_io_context();
                        tcp = new boost::asio::ip::tcp::socket(*io_context);
                    }

                    return tcp;
                }

            private:
                boost::asio::io_context *io_context;
                boost::asio::ip::udp::socket *udp;
                boost::asio::ip::tcp::socket *tcp;
            };
        }
    }
}
