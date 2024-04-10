#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/asio.hpp>
#include "socket_factory.cpp"
#include <core/core.hpp>

namespace fup
{
    namespace facade
    {
        namespace helper
        {
            class connection_factory
            {
            public:
                connection_factory(boost::shared_ptr<core::interface::checksum> checksum, int max_conn)
                {
                    checksum_service = checksum;
                    connections.resize(max_conn);
                }

                ~connection_factory()
                {
                    for (int i = 0; i < connections.size(); i++)
                    {
                        delete connections[i];
                    }
                }

                fup::core::connection *get_connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp)
                {
                    fup::core::connection *connection = new fup::core::connection(tcp, udp, checksum_service, ++connection_count);
                    connections[connection_count] = connection;
                    return connection;
                }

                fup::core::connection *get_connection(int id)
                {
                    return connections[id];
                }

                unsigned int get_connection_count()
                {
                    return connection_count;
                }

                void delete_connection(int id)
                {
                    delete connections[id];
                    connection_count--;
                }

            private:
                boost::shared_ptr<core::interface::checksum> checksum_service;
                std::vector<fup::core::connection *> connections;
                unsigned int connection_count;
                unsigned int connection_id_seq_num;
            };
        }
    }
}
