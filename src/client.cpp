#include "Client.hpp"

namespace fup {

    Client::Client(unsigned int v, std::string sl) : version{ (fup::Version)v }, saveLocation{ sl }, threadLimit{ FUP_DEFAULT_CLIENT_MAX_THREAD } {}

    Client::Client(unsigned int v, std::string sl, size_t mt) : version{ (fup::Version)v }, saveLocation{ sl }, threadLimit{ mt } {}

    Client::~Client() {}

    // Kind of private/underlying functions

    void Client::connectToDestination(std::string destUrl, std::string destPort) {
        try {

            int tcp_socketfd = initTcp(destUrl, destPort);
            conn = std::make_unique<Connection>(this->version, tcp_socketfd);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw e;
        }
    }

    void Client::disconnect()
    {
        conn.reset();
    }

    int Client::initTcp(std::string destUrl, std::string destPort)
    {
        int status;
        int socketfd;
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        struct addrinfo* res;
        status = getaddrinfo(destUrl.c_str(), destPort.c_str(), &hints, &res);
        if (status != 0)
        {
            std::ostringstream s;
            s << "socket fd error:  " << gai_strerror(status) << "\n ";
            throw std::runtime_error(s.str());
        }

        struct addrinfo* p;
        for (p = res; p != NULL; p = p->ai_next) // loop over addrinfos to get a socket
        {
            if ((socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != -1)
            {
                break;
            }
            std::cout << "can't connect socket. error: " << strerror(errno) << "\n";
        }
        if (p == NULL || socketfd == -1)
        {
            std::ostringstream s;
            s << "socket fd error:  " << strerror(errno) << "\n ";
            freeaddrinfo(res);
            throw std::runtime_error(s.str());
        }
        status = connect(socketfd, (struct sockaddr*)p, sizeof * p);
        if (status != 0)
        {
            std::ostringstream s;
            s << "connect error:  " << gai_strerror(status) << "\n ";
            freeaddrinfo(res);
            throw std::runtime_error(s.str());
        }
        printf("connected socket to %s:%s\n", destUrl.c_str(), destPort.c_str());

        freeaddrinfo(res);

        return socketfd;
    }


    // Operations possible from Client
    int Client::Upload(std::string destUrl, std::string destPort, std::string file_fullpath)
    {

        return 0;
    }
    int Client::Download(std::string destUrl, std::string destPort, std::string filename)
    {
        try {
            if (conn->getIsConnected()) throw std::logic_error("Client: invalid state. \"new_download_while_client_is_connected\"");
            connectToDestination(destUrl, destPort);

            Message request;
            fup::Download d_body = { .filename = filename };
            request.body = d_body.serialize();
            request.header = { .body_size = d_body.size(), .connectionId = conn->id, .version = this->version, .keyword = DOWNLOAD };

            Message response = conn->sendAndReceiveMessage(request);
            if (response.header.keyword == ABORT)
            {
                disconnect();
                errno = ECONNABORTED;
                std::ostringstream s;
                s << "server aborted the connection. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            if (response.header.keyword != FILE_INFO || conn->id != response.header.connectionId)
            {
                disconnect();
                errno = EPROTO;
                std::ostringstream s;
                s << "unexpected response from server. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            FileInfo file_info;
            int bytes_deserialized = file_info.deserialize(response.body);
            if (bytes_deserialized != response.body.size()) {
                disconnect();
                errno = ENODATA;
                std::ostringstream s;
                s << "could not parse file info completely. error: " << strerror(errno) << std::endl;
                throw std::runtime_error(s.str());
            }

            // thread_pool pool(max_thread_limit);
            // std::filesystem::path dir(this->save_location);
            // std::filesystem::path file(file_info.filename);
            // std::ofstream file_stream(dir / file, std::ios::binary | std::ios::trunc);
            // if (!file_stream) {
            //     disconnect();
            //     errno = ENODATA;
            //     std::ostringstream s;
            //     s << "can not open file. error: " << strerror(errno) << std::endl;
            //     throw std::runtime_error(s.str());
            // }
            return 0;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw e;
        }
    }
    //int ListFiles(Client* self, char* destUrl, int destPort, char** res);

}