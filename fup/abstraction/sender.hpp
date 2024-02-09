#pragma once

#ifndef FUP_ABSTRACTION_SENDER_HPP
#define FUP_ABSTRACTION_SENDER_HPP
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <fup/entity/entity.hpp>

namespace fup
{
    namespace abstraction
    {
        class sender
        {
        public:
            // These should be sent through UDP to maximize throughput
            virtual int send_packet(boost::scoped_ptr<boost::asio::ip::udp::socket> handler, fup::entity::packet packet) = 0;
            // Through TCP to notify server if any datagram is missing
            virtual int send_resend(boost::scoped_ptr<boost::asio::ip::tcp::socket> handler, int packageNumber) = 0;
            // Plan is to do this via TCP at the beginning to have Metadata on server and clients
            virtual int send_metadata(boost::scoped_ptr<boost::asio::ip::tcp::socket> handler, fup::entity::metadata metadata) = 0;
            // Again TCP handshake step for having keys and the generated key will be on each datagram so we could check which client is using and/or to have encryption
            virtual int send_key(boost::scoped_ptr<boost::asio::ip::tcp::socket> handler, std::string key) = 0;

            virtual ~sender() = default;
        };
    }
}
#endif