#include "client.hpp"

namespace fup {

    client::client(unsigned int version, std::string save_location) : version{ (fup::version)version }, save_location{ save_location }, max_thread_limit{ FUP_DEFAULT_MAX_THREAD } {}

    client::client(unsigned int version, std::string save_location, size_t max_threads) : version{ (fup::version)version }, save_location{ save_location }, max_thread_limit{ max_threads } {}

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
            freeaddrinfo(res);
            throw std::runtime_error(s.str());
        }
        status = connect(socket_fd, (struct sockaddr*)p, sizeof * p);
        if (status != 0)
        {
            std::ostringstream s;
            s << "connect error:  " << gai_strerror(status) << "\n ";
            freeaddrinfo(res);
            throw std::runtime_error(s.str());
        }
        printf("connected socket to %s:%s\n", destination_url.c_str(), destination_port.c_str());

        freeaddrinfo(res);

        return socket_fd;
    }

    void client::_connect(std::string destination_url, std::string destination_port) {
        try {
            
            int tcp_socket_fd = _init_socket(destination_url, destination_port, SOCK_STREAM);
            int udp_socket_fd = _init_socket(destination_url, destination_port, SOCK_DGRAM);
            conn = std::make_unique<connection>(tcp_socket_fd, udp_socket_fd);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw e;
        }
    }

    void client::_disconnect()
    {
        if (conn->get_is_connected()) {
            std::shared_ptr<message> msg = std::make_shared<message>();
            msg->header = { .body_size = 0, .connection_id = conn->id, .version = this->version, .keyword = ABORT };
            conn->send_message(msg, true);
        }
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
            download_body d_body = { .filename_size = filename.size(), .filename = filename };
            msg->body = d_body.serialize();
            msg->header = { .body_size = d_body.size(), .connection_id = conn->id, .version = this->version, .keyword = DOWNLOAD };
            conn->send_message(msg, true);

            *msg = conn->receive_message(true);
            if (msg->header.keyword == ABORT)
            {
                _disconnect();
                errno = ECONNABORTED;
                std::ostringstream s;
                s << "server aborted the connection. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            if (msg->header.keyword != FILE_INFO || conn->id != msg->header.connection_id)
            {
                _disconnect();
                errno = EPROTO;
                std::ostringstream s;
                s << "unexpected response from server. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            file_info_body file_info;
            int bytes_deserialized = file_info.deserialize(msg->body);
            if (bytes_deserialized != msg->body.size()) {
                _disconnect();
                errno = ENODATA;
                std::ostringstream s;
                s << "could not parse file info completely. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            thread_pool pool(max_thread_limit);
            std::filesystem::path dir(this->save_location);
            std::filesystem::path file(file_info.filename);
            std::ofstream file_stream(dir / file, std::ios::binary | std::ios::trunc);
            if (!file_stream) {
                _disconnect();
                errno = ENODATA;
                std::ostringstream s;
                s << "can not open file. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }
            return 0;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw e;
        }
    }
    //int ListFiles(Client* self, char* destination_url, int destination_port, char** res);

}