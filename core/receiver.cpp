#pragma once

namespace fup
{
    namespace core
    {
        virtual fup::core::entity::packet *receive_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler) const
        {
            fup::core::entity::packet r;
            handler->
        };
    }
}