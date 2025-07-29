#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET SocketHandle;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
inline int socket_close(SocketHandle s) {
    return closesocket(s);
}
#define socklen_t int
#define 
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
typedef int SocketHandle;
inline int socket_close(SocketHandle s) {
    return close(s);
}
#define INVALID_SOCKET_HANDLE -1
#endif

#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <set>
#include <iostream>
#include <fstream>
#include <future>

#define FUP_MAX_FILENAME_SIZE 255 // Maximum filename size is 255 bytes
#define FUP_DEFAULT_TCP_PORT 12420
#define FUP_DEFAULT_UDP_PORT 12421
#define FUP_DEFAULT_CLIENT_MAX_THREAD 4 // Should be enough for most of the cases
#define FUP_DEFAULT_SERVER_MAX_THREAD 32 // This value is the maximum number of threads that can be created by the server
#define FUP_CLIENT_MAX_CONNECTION 1 // Client should not make more than 1 connection with a server
#define FUP_DEFAULT_SERVER_MAX_CONNECTION 64 // This value is the maximum number of connections that can be handled by the server. This value needs to be lower than the maximum number of threads
#define FUP_PACKET_MESSAGE_SIZE 1200 // Each packet will have 1200 bytes. This value is chosen because it is relatively lower than the 1472 bytes which is the MTU for UDP/IP
#define FUP_PACKET_SIZE 1185 // Each message header has 15 bytes. So the maximum size of body can be 1185
#define FUP_PACKET_PAYLOAD_SIZE 1175 // In each packet there are sequence_id and checksum which takes another extra 10 bytes. So the maximum size of the payload can be 1175
#define FUP_MAX_RETRY_COUNT 32767

namespace fup
{
    typedef char Byte;

    typedef uint16_t Version;
    typedef uint16_t NameSize;
    typedef uint16_t FileCountSize;
    typedef uint16_t Port;

    typedef uint32_t Checksum;
    typedef uint32_t ConnectionId;
    typedef uint32_t PacketCount;

    typedef uint64_t BodySize;
    typedef uint64_t MessageSize;
    typedef uint64_t FileSize;
    typedef uint64_t SequenceId;
    enum Keyword : Byte
    {
        // Discrete basic request/response pair
        LIST_FILES, // TCP. Client to server. Response to this is FILES OR ABORT
        FILES,      // TCP. Server to client. Indicates success in the interaction. No response

        DOWNLOAD,      // TCP. Both directions. Request or response to get a file. Response to this is either UPLOAD or ABORT
        UPLOAD,        // TCP. Both directions. Request or response to upload a file. Response to this is either DOWNLOAD or ABORT
        RESEND,        // TCP. Both directions. Request to get a packet that has not been read. Response to this is either ACK or ABORT

        ACK,   // TCP. Both directions. Indicates that the request has been acknowledged by the other party. No response
        ABORT, // TCP. Both directions. Indicates that the request has not been acknowledged by the other party thus stopping connection. No response

        PACKET,      // UDP. Both directions. The uploading party should send the other one the parts of the file.
    };

    const std::string KeywordStrings[8] = {
        "LIST_FILES",
        "FILES",
        "DOWNLOAD",
        "UPLOAD",
        "RESEND",
        "ACK",
        "ABORT",
        "PACKET"
    };

    enum ConnectionStatus : Byte
    {
        DISCONNECTED,
        CONNECTED, // Idle
        DOWNLOADING,
        UPLOADING
    };

    struct StopSignal
    {
        bool stop;
        std::mutex mutex;
        StopSignal() : stop(false) {}
    };
}

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