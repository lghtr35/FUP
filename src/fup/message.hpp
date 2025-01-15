#ifndef FUP_MESSAGE_HPP
#define FUP_MESSAGE_HPP

#include "common.hpp"

namespace fup
{
    /*
     * Message is the structure of all messages
     * Message can have different versions with different fields.
     * Any can be assignable and can be parsed in its own way
     */
    struct message
    {
        header header;
        // Variable
        void *body;

        size_t size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
    };

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
        int size() { return sizeof(version) + sizeof(connection_id) + sizeof(keyword) + sizeof(size_t); }
        uint8_t serialize();
        int deserialize(uint8_t *data);
    };

    /*
     * ListFiles is for requesting to get all filenames and extensions from server
     * No body payload.
     */

    /*
     * Files is to response to ListFiles. All the file_names should be present
     */
    struct files
    {
        // Fixed
        unsigned int element_count;
        // Variable
        size_t *filename_sizes;
        uint8_t *filenames;
        int size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
    };

    /*
     * Download is to request for obtaining a file from server
     * It has filename
     * It has file extension
     * It has listen port for udp
     */
    struct download
    {
        // Fixed
        size_t filename_size;
        // Variable
        char *filename;
        int size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
    };

    /*
     * Upload is to request for to get permission to upload a new file to server
     * It has filename
     * It has file extension
     */
    struct upload
    {
        // Fixed
        size_t file_size;
        size_t filename_size;
        // Variable
        char *filename;
        int size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
    };

    /*
     * ResendPacket is to request to fetch a packet with specific id
     */
    struct resend
    {
        // Fixed
        int sequence_id;
        int size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
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

    struct packet
    {
        // Fixed
        int sequence_id; // to determine the ordering of data
        size_t checksum_size;
        size_t data_size;
        // Variable
        uint8_t *data;
        char *checksum;
        int size();
        uint8_t *serialize();
        int deserialize(uint8_t *data);
    };

}
#endif