#pragma once

#ifndef FUP_CORE_TRANSMISSION_RECEIVER_HPP
#define FUP_CORE_TRANSMISSION_RECEIVER_HPP
#include <core/entity/entity.hpp>
#include <boost/asio.hpp>

namespace fup
{
    namespace core
    {
        namespace transmission
        {
            class receiver_base
            {
            public:
                // Simply listen UDP after a handshake is done and get package.
                virtual fup::core::entity::packet *receive_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler) const = 0;
                // Over TCP, get Resend signal for a particular package.
                virtual int receive_resend(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const = 0;
                // Over TCP get metadata for the connection and the file.
                virtual fup::core::entity::metadata *receive_metadata(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const = 0;
                // Over TCP get key from opposite party to keep track of whom you are sending and/or encrypting messages
                virtual std::string receive_key(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const = 0;
                // Validate Checksum to be sure datagrams are intact
                virtual bool validate_checksum(unsigned char *data, unsigned char *checksum) const = 0;

                virtual ~receiver_base() = default;
            };
        }
    }
}
#endif