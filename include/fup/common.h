#ifndef FUP_COMMON_H
#define FUP_COMMON_H

#include <uuid/uuid.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

typedef unsigned int FUP_Version;
typedef unsigned int FUP_Size;
typedef uuid_t FUP_Connection_Id;
typedef enum FUP_Keyword
{
    // Discrete basic request/response pair
    LIST_FILES, // TCP. Client to server. Response to this is FILES
    FILES,      // TCP. Server to client. Indicates success in the interaction. If not success return ABORT

    DOWNLOAD,      // TCP. Client to server. Request to get a specific file. Response to this is either INFO or ABORT
    UPLOAD,        // TCP. Client to server. Informs to upload a file. Response to this is either INFO or ABORT
    RESEND_PACKET, // TCP. Both directions. Request to get a specific packet that has not been read with success. Response to this is either PACKET or ABORT

    ACK,   // TCP. Both directions. Indicates that the request has been acknowledged by the other party. No response
    ABORT, // TCP. Both directions. Indicates that the request has not been acknowledged by the other party thus stopping connection. No response

    CONNECT_UDP, // UDP. Client to server. Same connection id udp connection
    PACKET,      // UDP. Both directions. The uploading party should send the other one the parts of the file.
} FUP_Keyword;
#endif
