#ifndef FUP_MESSAGE_HPP
#define FUP_MESSAGE_HPP

#include "common.hpp"

namespace fup
{

    /*
     * Header is the fixed size_t part of each message
     * It contains information about the message
     */
    struct header
    {
        // Fixed
        version version;             // 1 int
        connection_id connection_id; // 16 unsigned char
        keyword keyword;             // 1 Keyword
        size_t body_size;            // 1 unsigned int
        size_t size() { return sizeof(version) + sizeof(connection_id) + sizeof(keyword) + sizeof(size_t); }
        uint8_t serialize();
        int deserialize(std::vector<uint8_t> data);
    };

    /*
     * Message is the structure of all messages
     * Message can have different versions with different fields.
     * Any can be assignable and can be parsed in its own way
     */
    struct message
    {
        header header;
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
    struct files_body
    {
        // Fixed
        size_t element_count;
        // Variable
        std::vector<size_t> filename_sizes;
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
    struct download_body
    {
        // Fixed
        size_t filename_size;
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
    struct upload_body
    {
        // Fixed
        size_t file_size;
        size_t filename_size;
        size_t packet_size;
        size_t packet_count;
        // Variable
        std::string filename;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };
    /*
     * File info is the message type which has the fundamental info about the file to be downloaded
    */
   struct file_info_body
   {
        // Fixed
        size_t file_size;
        size_t packet_size;
        size_t packet_count;
        size_t filename_size;
        // Variable
        std::string filename;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
   };

    /*
     * ResendPacket is to request to fetch a packet with specific id
     */
    struct resend_body
    {
        // Fixed
        int sequence_id;
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
     * Packet is the part of the file that is being transmitted
     *
     *
     */

    struct packet_body
    {
        // Fixed
        int sequence_id; // to determine the ordering of data
        size_t checksum_size;
        size_t data_size;
        // Variable
        std::vector<uint8_t> data;
        std::vector<uint8_t> checksum;
        size_t size();
        std::vector<uint8_t> serialize();
        int deserialize(std::vector<uint8_t> data);
    };

}
#endif