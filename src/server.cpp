#include "Server.hpp"

namespace fup {
    Server::Server(unsigned int v, int maxConn, std::string sl) {
        version = (fup::Version)v;
        maxConnections = maxConn;
        saveLocation = sl;
        threads = std::make_unique<Threadpool>(FUP_DEFAULT_SERVER_MAX_THREAD);
    }
}