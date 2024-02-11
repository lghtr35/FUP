#pragma once

#ifndef FUP_CORE_RECEIVER_HPP
#define FUP_CORE_RECEIVER_HPP
#include <core/transmission/transmission.hpp>

namespace fup
{
    namespace core
    {
        class receiver : transmission::receiver_base
        {
        public:
            virtual fup::core::entity::packet *receive_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler) const override;
            virtual int receive_resend(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const override;
            virtual fup::core::entity::metadata *receive_metadata(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const override;
            virtual std::string receive_key(boost::shared_ptr<boost::asio::ip::tcp::socket> handler) const override;
            virtual bool validate_checksum(unsigned char *data, unsigned char *checksum) const override;

            ~receiver(){};
        };
    }
}
#endif