
#include "response.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            std::vector<char> response::serialize()
            {
                // 4 bytes
                std::vector<char> status_bytes(sizeof(unsigned int));
                unsigned int status_be = htonl(status);
                std::memcpy(status_bytes.data(), &status_be, sizeof(unsigned int));

                // 4 bytes
                std::vector<char> connection_id_bytes(sizeof(unsigned int));
                unsigned int connection_id_be = htonl(connection_id);
                std::memcpy(connection_id_bytes.data(), &connection_id_be, sizeof(unsigned int));

                // 4 bytes
                std::vector<char> udp_port_bytes(sizeof(unsigned int));
                unsigned int udp_port_be = htonl(udp_port);
                std::memcpy(udp_port_bytes.data(), &udp_port_be, sizeof(unsigned int));

                std::vector<std::vector<char>> bytes_list({status_bytes,
                                                           connection_id_bytes,
                                                           udp_port_bytes});
                return fup::core::entity::serializer::concatenate_vectors<char>(bytes_list);
            }

            size_t response::deserialize(std::vector<char> &data)
            {
                size_t offset = 0;

                // Deserialize status
                std::memcpy(&status, data.data() + offset, sizeof(unsigned int));
                status = ntohl(status);
                offset += sizeof(unsigned int);

                // Deserialize connection_id
                std::memcpy(&connection_id, data.data() + offset, sizeof(unsigned int));
                connection_id = ntohl(connection_id);
                offset += sizeof(unsigned int);

                // Deserialize udp_port
                std::memcpy(&udp_port, data.data() + offset, sizeof(unsigned int));
                udp_port = ntohl(udp_port);
                offset += sizeof(unsigned int);

                return offset;
            }
        }
    }
}