#include "client.hpp"

namespace fup {

    client::client(int version, std::string save_location) : version{ version }, save_location{ save_location } {}

    client::~client() {}

    // Kind of private/underlying functions
    int client::_init_socket(std::string destination_url, std::string destination_port, int sock_type)
    {
        int status;
        int socket_fd;
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = sock_type;
        hints.ai_flags = AI_PASSIVE;

        struct addrinfo* res;
        status = getaddrinfo(destination_url.c_str(), destination_port.c_str(), &hints, &res);
        if (status != 0)
        {
            std::ostringstream s;
            s << "socket fd error:  " << gai_strerror(status) << "\n ";
            throw std::runtime_error(s.str());
        }

        struct addrinfo* p;
        for (p = res; p != NULL; p = p->ai_next) // loop over addrinfos to get a socket
        {
            if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != -1)
            {
                break;
            }
            std::cout << "can't connect socket. error: " << strerror(errno) << "\n";
        }
        if (p == NULL || socket_fd == -1)
        {
            std::ostringstream s;
            s << "socket fd error:  " << strerror(errno) << "\n ";
            throw std::runtime_error(s.str());
        }
        status = connect(socket_fd, (struct sockaddr*)p, sizeof * p);
        if (status != 0)
        {
            fprintf(stderr, "connect error: %s\n", strerror(status));
            return (-1);
        }
        printf("connected socket to %s:%d\n", destination_url, destination_port);

        freeaddrinfo(res);

        return socket_fd;
    }

    void client::_connect(std::string destination_url, std::string destination_port) {
        try {
            conn = std::make_unique<connection>();
            int tcp_socket_fd = _init_socket(destination_url, destination_port, SOCK_STREAM);
            int udp_socket_fd = _init_socket(destination_url, destination_port, SOCK_DGRAM);
            conn->set_tcp(tcp_socket_fd);
            conn->set_udp(udp_socket_fd);

            std::shared_ptr<message> tcp_message = std::make_shared<message>();
            tcp_message->header = { .body_size = 0, .version = this->version, .keyword = CONNECT_TCP };

            std::shared_ptr<message> udp_message = std::make_shared<message>();
            udp_message->header = { .body_size = 0, .version = this->version, .keyword = CONNECT_UDP };

            conn->send_message(tcp_message, true);

            *tcp_message = conn->receive_message(true);

            t1.join();
            t2.join();
            if (tcp_message->header.keyword != ACK || udp_message->header.keyword != ACK) {
                errno = EPROTO;
                std::ostringstream s;
                s << "server not accepting connection. error: " << strerror(errno) << "\n ";
                throw std::runtime_error(s.str());
            }

            conn->set_id(tcp_message->header.connection_id);
        }
        catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void client::_disconnect()
    {
        conn.reset();
    }

    // Operations possible from client
    int client::upload(std::string destination_url, std::string destination_port, std::string file_fullpath)
    {

        return 0;
    }
    int client::download(std::string destination_url, std::string destination_port, std::string filename)
    {
        try {
            _connect(destination_url, destination_port);

            std::shared_ptr<message> msg = std::make_shared<message>();
            download_body d_body = { .filename_size = filename.size(), .filename = filename.data() };
            msg->body = d_body.serialize();
            msg->header = { .body_size = d_body.size(), .connection_id = conn->id,.version = this->version, .keyword = DOWNLOAD };
            conn->send_message(msg, true);

            msg = conn->receive_message(true);
            if (msg->header.keyword == ABORT)
            {
                _disconnect();
                errno = ECONNABORTED;
                std::ostringstream s;
                s << "server aborted the connection. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            if (msg->header.keyword != ACK || conn->id == msg->header.connection_id)
            {
                _disconnect();
                errno = EPROTO;
                std::ostringstream s;
                s << "unexpected response from server. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }


            return 0;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    int ListFiles(Client* self, char* destination_url, int destination_port, char** res);

}