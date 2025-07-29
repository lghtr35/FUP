#include "Server.hpp"

namespace fup {
    Server::Server(Version v, uint32_t maxConn, uint16_t maxThreadPoolSize)
    : version(v),
      maxConnections(maxConn),
      maxThreadPoolSize(maxThreadPoolSize),
      listener(FUP_DEFAULT_TCP_PORT, FUP_DEFAULT_UDP_PORT) {
        // Initialize the listener socket
        listener.SetBlocking(true);
        // listener.Bind();
        // listener.Listen(maxConnections);

    }

}