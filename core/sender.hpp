#pragma once

#ifndef FUP_CORE_SENDER_HPP
#define FUP_CORE_SENDER_HPP
#include <core/transmission/transmission.hpp>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <core/entity/entity.hpp>

namespace fup
{
    namespace core
    {
        class sender : transmission::sender_base
        {
        public:
            int send_key(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const std::string key) const override;
            int send_metadata(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const entity::metadata &metadata) const override;
            int send_packet(boost::shared_ptr<boost::asio::ip::udp::socket> handler, const entity::packet &packet) const override;
            int send_resend(boost::shared_ptr<boost::asio::ip::tcp::socket> handler, const int packageNumber) const override;

        private:
            template <typename T>
            int send_tcp(boost::shared_ptr<boost::asio::ip::tcp::socket> &handler, const T &payload) const;
            template <typename T>
            int send_udp(boost::shared_ptr<boost::asio::ip::udp::socket> &handler, const T &payload) const;
            std::vector<uint8_t> serialize_payload(const std::string &p) const;
            template <typename T, typename = std::enable_if_t<std::is_base_of_v<entity::serializable, T>>>
            std::vector<uint8_t> serialize_payload(const T &p) const;
        };
    }
}

#endif