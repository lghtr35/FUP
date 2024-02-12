#pragma once

#ifndef FUP_CORE_TRANSMISSION_SENDER_HPP
#define FUP_CORE_TRANSMISSION_SENDER_HPP
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <core/entity/entity.hpp>

namespace fup
{
    namespace core
    {
        namespace transmission
        {
            class sender_base
            {
            public:
                // These should be sent through UDP to maximize throughput
                virtual int send_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler, const fup::core::entity::packet &packet) const = 0;
                // Through TCP to notify server if any datagram is missing
                virtual int send_resend(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const int packageNumber) const = 0;
                // Plan is to do this via TCP at the beginning to have Metadata on server and clients
                virtual int send_metadata(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const fup::core::entity::metadata &metadata) const = 0;
                // Again TCP handshake step for having keys and the generated key will be on each datagram so we could check which client is using and/or to have encryption
                virtual int send_key(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const std::string key) const = 0;

                virtual ~sender_base() = default;
            };
        }
    }
}
#endif