#pragma once

#ifndef FUP_ABSTRACTION_RECEIVER_HPP
#define FUP_ABSTRACTION_RECEIVER_HPP
#include <fup/entity/entity.hpp>
#include <boost/asio.hpp>

namespace fup
{
    namespace abstraction
    {
        class receiver
        {
        public:
            // Simply listen UDP after a handshake is done and get package.
            virtual fup::entity::packet receive_packet(boost::asio::ip::udp::socket handler);
            // Over TCP, get Resend signal for a particular package.
            virtual int receive_resend(boost::asio::ip::tcp::socket handler);
            // Over TCP get metadata for the connection and the file.
            virtual fup::entity::metadata receive_metadata(boost::asio::ip::tcp::socket handler);
            // Over TCP get key from opposite party to keep track of whom you are sending and/or encrypting messages
            virtual std::string receive_key(boost::asio::ip::tcp::socket handler);
            // Validate Checksum to be sure datagrams are intact
            virtual bool validate_checksum(unsigned char *data, unsigned char *checksum);
        };
    }
}
#endif