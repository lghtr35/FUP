#ifndef FUP_MESSAGE_H
#define FUP_MESSAGE_H

#include "common.h"

/*
 * Message is the structure of all messages
 * Message can have different versions with different fields.
 * Any can be assignable and can be parsed in its own way
 */
typedef struct FUP_Message
{
    FUP_M_Header header;
    // Variable
    void *body;
} FUP_Message;

int FUP_Message_Serialize(FUP_Message *self, char *buffer);
int FUP_Message_Deserialize(char **data, FUP_Message *message);
void FUP_Message_Delete(FUP_Message *self);

int FUP_Message_Send(int socket_fd, const FUP_Message request);
int FUP_Message_Receive(int socket_fd, FUP_Message *response);

/*
 * Header is the fixed size part of each message
 * It contains information about the message
 */
typedef struct FUP_M_Header
{
    // Fixed
    FUP_Version version;             // 1 int
    FUP_Connection_Id connection_id; // 16 unsigned char
    FUP_Keyword keyword;             // 1 FUP_Keyword
    FUP_Size body_size;              // 1 unsigned int
} FUP_M_Header;
int FUP_M_Header_Size()
{
    return sizeof(FUP_Version) + sizeof(FUP_Connection_Id) + sizeof(FUP_Keyword) + sizeof(FUP_Size);
};
int FUP_M_Header_Serialize(FUP_M_Header *self, char *buffer);
int FUP_M_Header_Deserialize(char **data, FUP_M_Header *message);
void FUP_M_Header_Delete(FUP_M_Header *self);

/*
 * ListFiles is for requesting to get all filenames and extensions from server
 * No body payload.
 */

/*
 * Files is to response to ListFiles. All the file_names should be present
 */
typedef struct FUP_M_Files
{
    // Fixed
    unsigned int element_count;
    // Variable
    FUP_Size *filename_sizes;
    char **filenames;

} FUP_M_Files;

char *FUP_M_Files_Serialize(FUP_M_Files *self);
FUP_M_Files FUP_M_Files_Deserialize(char **data);
void FUP_M_Files_Delete(FUP_M_Files *self);

/*
 * Download is to request for obtaining a file from server
 * It has filename
 * It has file extension
 * It has listen port for udp
 */
typedef struct FUP_M_Download
{
    // Fixed
    FUP_Size filename_size;
    // Variable
    char *filename;
} FUP_M_Download;

int FUP_M_Download_Size(FUP_M_Download *self);
char *FUP_M_Download_Serialize(FUP_M_Download *self);
FUP_M_Download FUP_M_Download_Deserialize(char **data);
void FUP_M_Download_Delete(FUP_M_Download *self);

/*
 * Upload is to request for to get permission to upload a new file to server
 * It has filename
 * It has file extension
 */
typedef struct FUP_M_Upload
{
    // Fixed
    FUP_Size filename_size;
    // Variable
    char *filename;
} FUP_M_Upload;

int FUP_M_Upload_Size(FUP_M_Upload *self);
char *FUP_M_Upload_Serialize(FUP_M_Upload *self);
FUP_M_Upload FUP_M_Upload_Deserialize(char **data);
void FUP_M_Upload_Delete(FUP_M_Upload *self);

/*
 * ResendPacket is to request to fetch a packet with specific id
 */
typedef struct FUP_M_ResendPacket
{
    // Fixed
    int sequence_id;
} FUP_M_ResendPacket;

int FUP_M_ResendPacket_Size(FUP_M_ResendPacket *self);
char *FUP_M_ResendPacket_Serialize(FUP_M_ResendPacket *self);
FUP_M_ResendPacket FUP_M_ResendPacket_Deserialize(char **data);
void FUP_M_ResendPacket_Delete(FUP_M_ResendPacket *self);

/*
 * Ack is to inform the corresponding party that request has been acknowledged
 * Does not have a body
 */

/*
 * Abort is to inform the corresponding party that request has not been acknowledged
 * Does not have a body
 */

/*
 * Packet is the part of the file that is being transmitted
 *
 *
 */

typedef struct FUP_M_Packet
{
    // Fixed
    int sequence_id; // to determine the ordering of data
    FUP_Size checksum_size;
    FUP_Size data_size;
    // Variable
    char **data;
    char *checksum;
} FUP_M_Packet;

int FUP_M_Packet_Size(FUP_M_Packet *self);
char *FUP_M_Packet_Serialize(FUP_M_Packet *self);
FUP_M_Packet FUP_M_Packet_Deserialize(char **data);
void FUP_M_Packet_Delete(FUP_M_Packet *self);

#endif