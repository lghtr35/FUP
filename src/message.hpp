#ifndef FUP_MESSAGE_HPP
#define FUP_MESSAGE_HPP

#include "Common.hpp"

namespace fup
{

    /*
     * Header is the fixed size_t part of each message
     * It contains information about the message
     */
    struct Header
    {
        // Fixed
        Version version;             // 1 int
        ConnectionId connectionId; // 16 unsigned char
        Keyword keyword;             // 1 Keyword
        size_t body_size;            // 1 unsigned int
        size_t size() { return sizeof(Version) + sizeof(ConnectionId) + sizeof(Keyword) + sizeof(size_t); }
        uint8_t serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * Message is the structure of all messages
     * Message can have different Versions with different fields.
     * Any can be assignable and can be parsed in its own way
     */
    struct Message
    {
        Header header;
        // Variable
        std::vector<uint8_t> body;

        size_t size() {return header.size() + header.body_size;};
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * ListFiles is for requesting to get all filenames and extensions from server
     * No body payload.
     */

    /*
     * Files is to response to ListFiles. All the file_names should be present
     */
    struct Files
    {
        // Fixed
        size_t elemCount;
        // Variable
        std::vector<size_t> nameSizes;
        std::vector<std::string> filenames;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * Download is to request for obtaining a file from server
     * It has filename
     * It has file extension
     * It has listen port for udp
     */
    struct Download
    {
        // Fixed
        // Variable
        std::string filename;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * Upload is to request for to get permission to upload a new file to server
     * It has filename
     * It has file extension
     */
    struct Upload
    {
        // Fixed
        size_t fileSize;
        size_t packetSize;
        size_t packetCount;
        // Variable
        std::string filename;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };
    /*
     * File info is the message type which has the fundamental info about the file to be downloaded
    */
   struct FileInfo
   {
        // Fixed
        size_t fileSize;
        size_t packetSize;
        size_t packetCount;
        // Variable
        std::string filename;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
   };

    /*
     * Resend is to request to fetch a packet with specific id
     */
    struct Resend
    {
        // Fixed
        SequenceId seqId;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * Ack is to inform the corresponding party that request has been acknowledged
     * Does not have a body
     */

    /*
     * Abort is to inform the corresponding party that request has not been acknowledged
     * Does not have a body
     */

    /*
     * Packet is the part of the file that is being transmitted it has to be a fixed size since when combined with header it should be equal to packet_size in upload_body or file_info_body
     *
     *
     */

    struct Packet
    {
        // Fixed
        SequenceId seqId; // to determine the ordering of data
        int16_t checksum;
        // Variable
        std::vector<uint8_t> data;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

}
#endif