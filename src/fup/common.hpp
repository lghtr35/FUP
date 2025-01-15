#ifndef FUP_COMMON_HPP
#define FUP_COMMON_HPP

#include <uuid/uuid.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>

namespace fup
{
    typedef unsigned int version;
    typedef uuid_t connection_id;
    typedef enum keyword
    {
        // Discrete basic request/response pair
        LIST_FILES, // TCP. Client to server. Response to this is FILES OR ABORT
        FILES,      // TCP. Server to client. Indicates success in the interaction. No response

        DOWNLOAD,      // TCP. Client to server. Request to get a specific file. Response to this is either ACK or ABORT
        UPLOAD,        // TCP. Client to server. Informs to upload a file. Response to this is either ACK or ABORT
        RESEND_PACKET, // TCP. Both directions. Request to get a specific packet that has not been read with success. Response to this is either PACKET or ABORT

        ACK,   // TCP. Both directions. Indicates that the request has been acknowledged by the other party. No response
        ABORT, // TCP. Both directions. Indicates that the request has not been acknowledged by the other party thus stopping connection. No response

        CONNECT_UDP, // UDP. Client to server. Same connection id udp connection
        PACKET,      // UDP. Both directions. The uploading party should send the other one the parts of the file.
    } keyword;
}
#endif
