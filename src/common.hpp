#ifndef FUP_COMMON_HPP
#define FUP_COMMON_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unordered_map>

#ifndef FUP_DEFAULT_CLIENT_MAX_THREAD
#define FUP_DEFAULT_CLIENT_MAX_THREAD 8
#endif
#ifndef FUP_DEFAULT_SERVER_MAX_THREAD
#define FUP_DEFAULT_SERVER_MAX_THREAD 32
#endif

namespace fup
{

    typedef uint16_t Version;
    typedef uint32_t ConnectionId;
    typedef uint64_t SequenceId;
    typedef enum Keyword
    {
        // Discrete basic request/response pair
        LIST_FILES, // TCP. Client to server. Response to this is FILES OR ABORT
        FILES,      // TCP. Server to client. Indicates success in the interaction. No response

        DOWNLOAD,      // TCP. Client to server. Request to get a specific file. Response to this is either FILE_INFO or ABORT
        UPLOAD,        // TCP. Client to server. Request to upload a file. Response to this is either ACK or ABORT
        FILE_INFO,     // TCP. Server to client. Response to download file which holds all the info about file. Response to this is ACK or ABORT.
        RESEND,        // TCP. Both directions. Request to get a specific packet that has not been read with success. Response to this is either PACKET or ABORT

        ACK,   // TCP. Both directions. Indicates that the request has been acknowledged by the other party. No response
        ABORT, // TCP. Both directions. Indicates that the request has not been acknowledged by the other party thus stopping connection. No response

        PACKET,      // UDP. Both directions. The uploading party should send the other one the parts of the file.
    } Keyword;
}
#endif

// Basic Download Flow
// Client connects server via TCP
// Server responds with ACK which has connection_id
// Client sets up the connection then requests download
// Server returns FILE_INFO to inform Client about the file and the transmission
// Client sends ACK through UDP with connection_id so Server recognizes it 
// Server starts sending packets to the Client through its UDP
// If any expected packets not arrived, Client sends resend_packet via TCP
// Server readds that packet to the queue and continues sending
// If server sends TCP ACK or ABORT while this happens connection is closed (either in success or failure)
// If client sends TCP ACK or ABORT while this happens connection is closed (either in success or failure)