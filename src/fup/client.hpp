#ifndef FUP_CLIENT_H
#define FUP_CLIENT_H

#include "common.hpp"
#include "connection.hpp"

namespace fup
{
    /*
     * Can download one file at a time
     * Might need refactoring to allow more versions of FUP to be implemented as submodules
     */

    class client
    {
    public:
        client(int version, char *save_location);
        ~client();

        int upload(char *destination_url, int destination_port, char *file_fullpath);
        int download(char *destination_url, int destination_port, char *filename);
        int list_files(char *destination_url, int destination_port, char **res);

    private:
        version version;
        char *save_location;
        connection *connection;
        int connect(int ip_family, char *destination_url, unsigned short destination_port, int sock_type);
        int disconnect();
    };
}

#endif