#ifndef FUP_COMMON_HPP
#define FUP_COMMON_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace fup
{
    typedef unsigned int version;
    typedef unsigned int connection_id;
    typedef enum keyword
    {
        // Discrete basic request/response pair
        CONNECT_TCP, // TCP. Client to server. Response is ACK or ABORT
        CONNECT_UDP, // UDP. Client to server. Needs to be same conn id with CONNECT_TCP. Response is ACK or ABORT

        LIST_FILES, // TCP. Client to server. Response to this is FILES OR ABORT
        FILES,      // TCP. Server to client. Indicates success in the interaction. No response

        DOWNLOAD,      // TCP. Client to server. Request to get a specific file. Response to this is either FILE_INFO or ABORT
        UPLOAD,        // TCP. Client to server. Request to upload a file. Response to this is either ACK or ABORT
        FILE_INFO,     // TCP. Server to client. Response to download file which holds all the info about file. Response to this is ACK or ABORT.
        RESEND_PACKET, // TCP. Both directions. Request to get a specific packet that has not been read with success. Response to this is either PACKET or ABORT

        ACK,   // TCP. Both directions. Indicates that the request has been acknowledged by the other party. No response
        ABORT, // TCP. Both directions. Indicates that the request has not been acknowledged by the other party thus stopping connection. No response

        PACKET,      // UDP. Both directions. The uploading party should send the other one the parts of the file.
    } keyword;
}
#endif
