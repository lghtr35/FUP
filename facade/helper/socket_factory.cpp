

#include "socket_factory.hpp"

namespace fup
{
    namespace facade
    {
        namespace helper
        {
            socket_factory::socket_factory(boost::asio::io_context &ctx)
            {
                io_context = &ctx;
            }

            socket_factory::~socket_factory()
            {
                // io_context memory should be managed on parent class
            }

            boost::asio::ip::udp::socket *socket_factory::get_udp()
            {
                return new boost::asio::ip::udp::socket(*io_context);
            }

            boost::asio::ip::udp::socket *socket_factory::get_udp(int port)
            {
                return new boost::asio::ip::udp::socket(*io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
            }

            boost::asio::ip::tcp::socket *socket_factory::get_tcp()
            {
                return new boost::asio::ip::tcp::socket(*io_context);
            }

        };
    }
}
