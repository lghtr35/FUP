
#include "connection_factory.hpp"

namespace fup
{
    namespace facade
    {
        namespace helper
        {
            connection_factory::connection_factory() {}

            connection_factory::~connection_factory()
            {
                for (int i = 0; i < connections.size(); i++)
                {
                    delete connections[i];
                }
            }

            fup::core::connection *connection_factory::get_connection(boost::asio::ip::tcp::socket *tcp, boost::asio::ip::udp::socket *udp)
            {
                mutex.lock();
                int free_id = 0;
                for (; free_id < connections.size(); free_id++)
                {
                    if (connections[free_id] == nullptr)
                        break;
                }
                fup::core::connection *connection = new fup::core::connection(tcp, udp, free_id);
                connections[free_id] = connection;
                connection_count++;
                mutex.unlock();
                return connection;
            }

            fup::core::connection *connection_factory::get_connection(int id)
            {
                return connections[id];
            }

            unsigned int connection_factory::get_connection_count()
            {
                return connection_count;
            }

            void connection_factory::delete_connection(int id)
            {
                mutex.lock();
                delete connections[id];
                connection_count--;
                mutex.unlock();
            }
        }
    }
}
