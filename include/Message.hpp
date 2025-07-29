#pragma once

#include "Common.hpp"

namespace fup
{

    class Body {
    public:
        Body(std::vector<Byte>) {};
        Body() = default;
        virtual ~Body() = default;

        virtual MessageSize size() const { return 0; };
        virtual std::vector<Byte> serialize() const { return std::vector<Byte>(); };
    };

    /*
     * Header is the fixed size part of each message
     * It contains information about the message
     */
    class Header
    {
    public:
        Version version;              // 1 uint16_t
        ConnectionId connectionId;    // 1 uint32_t
        Keyword keyword;              // 1 uint8_t
        BodySize bodySize;            // 1 uint64_t
        Header(const std::vector<Byte>& data);
        Header() : version(0), connectionId(0), keyword(ACK), bodySize(0) {};
        Header(Version v, ConnectionId cid, Keyword kw, BodySize bsize = 0) : version(v), connectionId(cid), keyword(kw), bodySize(bsize) {};
        static MessageSize size() { return sizeof(Keyword) + sizeof(BodySize) + sizeof(Version) + sizeof(ConnectionId); };
        std::vector<Byte> serialize() const;

    };

    /*
     * Message is the wrapper of all kinds of messages
     * Message can have different Versions with different fields.
     * Any can be assignable and can be parsed in its own way
     */
    class Message
    {
    public:
        Header header;
        // Variable
        std::shared_ptr<Body> body;
        Message(const Header& header, const std::vector<Byte>& data);
        Message(const std::vector<Byte>& data);
        Message() : body(nullptr) {};
        Message(Header h, std::shared_ptr<Body> b) : header(h), body(b) {
            header.bodySize = body->size();
        };
        MessageSize size() const { return header.size() + body->size(); };
        std::vector<Byte> serialize() const;
        template <class T, typename std::enable_if<std::is_base_of<Body, T>::value, int>::type = 0>
        std::shared_ptr<T> getBody() {
            if (!body) {
                return nullptr;
            }
            return std::static_pointer_cast<T>(body);
        }
    };

    /*
     * ListFiles is for requesting to get all filenames and extensions from server
     * No body payload.
     */

     /*
      * Files is to response to ListFiles. All the file_names should be present
      */
    class Files : public Body
    {
    public:
        // Fixed
        FileCountSize elemCount;
        // Variable
        std::vector<NameSize> nameSizes;
        std::vector<std::string> filenames;
        Files(const std::vector<Byte>& data);
        Files() : elemCount(0) {};
        Files(FileCountSize count, std::vector<NameSize> sizes, std::vector<std::string> names) : elemCount(count), nameSizes(sizes), filenames(names) {};
        MessageSize size() const override;
        std::vector<Byte> serialize() const override;
    };

    /*
     * Download is to request for obtaining a file from server
     * It has filename
     * It has listen port for udp
     */
    class Download : public Body
    {
    public:
        // Fixed
        Port udpPort;
        NameSize filenameSize;
        // Variable
        std::string filename;
        Download(const std::vector<Byte>& data);
        Download() : udpPort(FUP_DEFAULT_UDP_PORT) {};
        Download(Port port, std::string name) : udpPort(port), filenameSize(name.size()), filename(name) {};
        MessageSize size() const;
        std::vector<Byte> serialize() const;
    };

    /*
     * Upload is to request for to get permission to upload a new file to server
     * It has filename
     * It has file extension
     */
    class Upload : public Body
    {
    public:
        // Fixed
        Port udpPort;
        FileSize fileSize;
        PacketCount packetCount;
        NameSize filenameSize;
        // Variable
        std::string filename;
        Upload(const std::vector<Byte>& data);
        Upload() : udpPort(FUP_DEFAULT_UDP_PORT) {};
        Upload(Port port, FileSize size, PacketCount count, NameSize nameSize, std::string name) : udpPort(port), fileSize(size), packetCount(count), filenameSize(nameSize), filename(name) {};
        MessageSize size() const;
        std::vector<Byte> serialize() const;
    };
    /*
     * Resend is to request to fetch a packet with specific id
     */
    class Resend : public Body
    {
    public:
        // Fixed
        SequenceId seqId;
        Resend(const std::vector<Byte>& data);
        Resend() : seqId(0) {};
        Resend(SequenceId id) : seqId(id) {};
        MessageSize size() const;
        std::vector<Byte> serialize() const;
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
       */
    class Packet : public Body
    {
    public:
        // Fixed
        SequenceId seqId; // to determine the ordering of data
        Checksum checksum; // to determine the integrity of data
        // Variable
        std::vector<Byte> payload;
        Packet(const std::vector<Byte>& data);
        Packet(SequenceId id, Checksum sum, std::vector<Byte> data) : seqId(id), checksum(sum), payload(data) {};
        // Packet(SequenceId id, Checksum sum) : seqId(id), checksum(sum) {};
        Packet() : seqId(0), checksum(0) {};
        MessageSize size() const;
        std::vector<Byte> serialize() const;
    };

    class BodyFactory {
    public:
        static std::shared_ptr<Body> createBody(Keyword, std::vector<Byte>);
    };


    class PacketQueue {
    public:
        PacketQueue() {
            packetQueue = std::make_shared<std::queue<Message>>();
            packetQueueMutex = std::make_shared<std::mutex>();
        }

        void Push(const Message& packet);
        Message Pop();
        bool IsEmpty() const;
    private:
        std::shared_ptr<std::queue<Message>> packetQueue;
        std::shared_ptr<std::mutex> packetQueueMutex;
    };

    class QueueMap {
    public:
        QueueMap() {
            qMap = std::make_shared<std::unordered_map<ConnectionId, std::shared_ptr<PacketQueue>>>();
            mutex = std::make_shared<std::mutex>();
        }

        std::shared_ptr<PacketQueue> operator[](const ConnectionId& id);
        void Remove(const ConnectionId& id);
        std::unordered_map<ConnectionId, std::shared_ptr<PacketQueue>>::iterator Begin() { return qMap->begin(); }
        std::unordered_map<ConnectionId, std::shared_ptr<PacketQueue>>::iterator End() { return qMap->end(); }
        std::unordered_map<ConnectionId, std::shared_ptr<PacketQueue>>::iterator Find(ConnectionId id) { return qMap->find(id); }
    private:
        std::shared_ptr<std::unordered_map<ConnectionId, std::shared_ptr<PacketQueue>>> qMap;
        std::shared_ptr<std::mutex> mutex;
    };
}