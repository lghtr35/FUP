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
                connection_factory(boost::shared_ptr<boost::asio::io_context> ctx, boost::shared_ptr<core::interface::checksum> checksum, int max_conn)
                {
                    socket_factory = new helper::socket_factory(ctx);
                    checksum_service = checksum;
                    connections.resize(max_conn);
                }

                ~connection_factory()
                {
                    // Dont delete io_context cause it is shared
                    for (int i = 0; i < connections.size(); i++)
                    {
                        delete connections[i];
                    }
                    delete socket_factory;
                }

                fup::core::connection *get_connection()
                {
                    fup::core::connection *connection = new fup::core::connection(socket_factory->get_tcp(), socket_factory->get_udp(), checksum_service, ++connection_count);
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
                helper::socket_factory *socket_factory;
                boost::shared_ptr<core::interface::checksum> checksum_service;
                std::vector<fup::core::connection *> connections;
                unsigned int connection_count;
                unsigned int connection_id_seq_num;
            };
        }
    }
}
