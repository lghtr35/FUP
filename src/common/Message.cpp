#include "Message.hpp"

namespace fup {
    std::vector<Byte> Header::serialize() const {
        std::vector<Byte> buf(size());
        int offset = 0;
        Version v = htons(version);
        memcpy(buf.data(), &v, sizeof(Version));
        offset += sizeof(Version);

        ConnectionId cid = htonl(connectionId);
        memcpy(buf.data() + offset, &cid, sizeof(ConnectionId));
        offset += sizeof(ConnectionId);

        Byte k = (Byte)keyword;
        memcpy(buf.data() + offset, &k, sizeof(Byte));
        offset += sizeof(Byte);

        BodySize bs = htonl(bodySize);
        memcpy(buf.data() + offset, &bs, sizeof(BodySize));
        offset += sizeof(BodySize);

        return buf;
    }

    Header::Header(const std::vector<Byte>& data) {
        if (data.size() < size()) {
            throw std::runtime_error("fup::Header: Insufficient data");
        }
        int offset = 0;
        memcpy(&version, data.data(), sizeof(Version));
        version = ntohs(version);
        offset += sizeof(Version);

        memcpy(&connectionId, data.data() + offset, sizeof(ConnectionId));
        connectionId = ntohl(connectionId);
        offset += sizeof(ConnectionId);

        Byte k;
        memcpy(&k, data.data() + offset, sizeof(Byte));
        keyword = (Keyword)k;
        offset += sizeof(Byte);

        memcpy(&bodySize, data.data() + offset, sizeof(BodySize));
        bodySize = ntohl(bodySize);
        offset += sizeof(BodySize);
    }

    std::vector<Byte> Message::serialize() const {
        std::vector<Byte> buf(size());
        int offset = 0;
        std::vector<Byte> h = header.serialize();
        memcpy(buf.data(), h.data(), h.size());
        offset += h.size();

        std::vector<Byte> b = body->serialize();
        memcpy(buf.data() + offset, b.data(), b.size());
        offset += b.size();

        return buf;
    }

    Message::Message(const Header& h,const std::vector<Byte>& data) {
        header = h;

        if (data.size() < header.bodySize) {
            throw std::runtime_error("fup::Message: Insufficient data");
        }
        body = BodyFactory::createBody(header.keyword, std::vector<Byte>(data.begin() + Header::size(), data.end()));
    }

    Message::Message(const std::vector<Byte>& data) {
        if (data.size() < Header::size()) {
            throw std::runtime_error("fup::Message: Insufficient data");
        }
        header = Header(data);
        if (data.size() - Header::size() < header.bodySize) {
            throw std::runtime_error("fup::Message: Insufficient data");
        }
        body = BodyFactory::createBody(header.keyword, std::vector<Byte>(data.begin() + Header::size(), data.end()));
    }

    MessageSize Files::size() const {
        int total = 0;
        for (int s : this->nameSizes) {
            total += s;
        }
        return total + ((nameSizes.size() + 1) * sizeof(Byte));
    }

    std::vector<Byte> Files::serialize() const {
        int n = size();
        std::vector<Byte> buf(n);
        int offset = 0;
        Byte temp = htons(elemCount);
        memcpy(buf.data(), &elemCount, sizeof(FileCountSize));
        offset += sizeof(FileCountSize);

        for (uint16_t s : nameSizes) {
            temp = htons(s);
            memcpy(buf.data() + offset, &temp, sizeof(NameSize));
            offset += sizeof(NameSize);
        }

        for (std::string name : filenames) {
            memcpy(buf.data() + offset, name.c_str(), name.length());
            offset += name.length();
        }

        return buf;
    }

    Files::Files(const std::vector<Byte>& data) {
        if (data.size() < sizeof(FileCountSize)) {
            throw std::runtime_error("fup::Files: Insufficient data");
        }
        int offset = 0;
        memcpy(&elemCount, data.data(), sizeof(FileCountSize));
        elemCount = ntohs(elemCount);
        offset += sizeof(FileCountSize);

        if (data.size() - sizeof(FileCountSize) < elemCount * sizeof(NameSize)) {
            throw std::runtime_error("fup::Files: Insufficient data");
        }

        nameSizes.resize(elemCount);
        int totalStringLen = 0;
        for (int i = 0; i < elemCount; i++) {
            memcpy(nameSizes.data() + i, data.data() + offset, sizeof(NameSize));
            nameSizes[i] = ntohs(nameSizes[i]);
            totalStringLen += nameSizes[i];
            offset += sizeof(NameSize);
        }

        if (totalStringLen + offset > static_cast<int>(data.size())) {
            throw std::runtime_error("fup::Files: Insufficient data");
        }

        filenames.resize(elemCount);
        for (int i = 0; i < elemCount; i++) {
            filenames[i].resize(nameSizes[i]);
            memcpy(filenames[i].data(), data.data() + offset, nameSizes[i]);
            offset += nameSizes[i];
        }
    }

    MessageSize Download::size() const {
        return sizeof(Port) + sizeof(NameSize) + filename.length();
    }

    std::vector<Byte> Download::serialize() const {
        std::vector<Byte> buf(size());
        int offset = 0;
        Port p = htons(udpPort);
        memcpy(buf.data(), &p, sizeof(Port));
        offset += sizeof(Port);

        NameSize ns = htons(filename.length());
        memcpy(buf.data() + offset, &ns, sizeof(NameSize));
        offset += sizeof(NameSize);

        memcpy(buf.data() + offset, filename.c_str(), filename.length());
        offset += filename.length();

        return buf;
    }

    Download::Download(const std::vector<Byte>& data) {
        if (data.size() < sizeof(Port) + sizeof(NameSize)) {
            throw std::runtime_error("fup::Download: Insufficient data");
        }
        int offset = 0;
        memcpy(&udpPort, data.data(), sizeof(Port));
        udpPort = ntohs(udpPort);
        offset += sizeof(Port);

        NameSize ns;
        memcpy(&ns, data.data() + offset, sizeof(NameSize));
        ns = ntohs(ns);
        offset += sizeof(NameSize);

        if (ns == 0 || data.size() - offset < ns) {
            throw std::runtime_error("fup::Download: Insufficient data");
        }

        filename.resize(ns);
        memcpy(filename.data(), data.data() + offset, ns);
        offset += ns;
    }

    MessageSize Upload::size() const {
        return sizeof(Port) + sizeof(FileSize) + sizeof(PacketCount) + sizeof(NameSize) + filename.length();
    }

    std::vector<Byte> Upload::serialize() const {
        std::vector<Byte> buf(size());
        int offset = 0;
        Port p = htons(udpPort);
        memcpy(buf.data(), &p, sizeof(Port));
        offset += sizeof(Port);

        FileSize fs = htonll(fileSize);
        memcpy(buf.data() + offset, &fs, sizeof(FileSize));
        offset += sizeof(FileSize);

        PacketCount pc = htonl(packetCount);
        memcpy(buf.data() + offset, &pc, sizeof(PacketCount));
        offset += sizeof(PacketCount);

        NameSize ns = htons(filename.length());
        memcpy(buf.data() + offset, &ns, sizeof(NameSize));
        offset += sizeof(NameSize);

        memcpy(buf.data() + offset, filename.c_str(), filename.length());
        offset += filename.length();

        return buf;
    }

    Upload::Upload(const std::vector<Byte>& data) {
        if (data.size() < sizeof(Port) + sizeof(FileSize) + sizeof(PacketCount) + sizeof(NameSize)) {
            throw std::runtime_error("fup::Upload: Insufficient data");
        }
        int offset = 0;
        memcpy(&udpPort, data.data(), sizeof(Port));
        udpPort = ntohs(udpPort);
        offset += sizeof(Port);

        memcpy(&fileSize, data.data() + offset, sizeof(FileSize));
        fileSize = ntohll(fileSize);
        offset += sizeof(FileSize);

        memcpy(&packetCount, data.data() + offset, sizeof(PacketCount));
        packetCount = ntohl(packetCount);
        offset += sizeof(PacketCount);

        NameSize ns;
        memcpy(&ns, data.data() + offset, sizeof(NameSize));
        ns = ntohs(ns);
        offset += sizeof(NameSize);

        if (ns == 0 || data.size() - offset < ns) {
            throw std::runtime_error("fup::Upload: Insufficient data");
        }

        filename.resize(ns);
        memcpy(filename.data(), data.data() + offset, ns);
        offset += ns;
    }

    MessageSize Resend::size() const {
        return sizeof(SequenceId);
    }

    std::vector<Byte> Resend::serialize() const {
        std::vector<Byte> buf(size());
        SequenceId s = htonll(seqId);
        memcpy(buf.data(), &s, sizeof(SequenceId));

        return buf;
    }

    Resend::Resend(const std::vector<Byte>& data) {
        if (data.size() < sizeof(SequenceId)) {
            throw std::runtime_error("fup::Resend: Insufficient data");
        }
        memcpy(&seqId, data.data(), sizeof(SequenceId));
        seqId = ntohll(seqId);
    }

    MessageSize Packet::size() const {
        return FUP_PACKET_SIZE;
    }

    std::vector<Byte> Packet::serialize() const {
        std::vector<Byte> buf(size());
        int offset = 0;
        SequenceId s = htonll(seqId);
        memcpy(buf.data(), &s, sizeof(SequenceId));
        offset += sizeof(SequenceId);

        Checksum c = htons(checksum);
        memcpy(buf.data() + offset, &c, sizeof(Checksum));
        offset += sizeof(Checksum);

        memcpy(buf.data() + offset, payload.data(), payload.size());

        return buf;
    }

    Packet::Packet(const std::vector<Byte>& data) {
        if (data.size() < sizeof(SequenceId) + sizeof(Checksum)) {
            throw std::runtime_error("fup::Packet: Insufficient data");
        }
        int offset = 0;
        memcpy(&seqId, data.data(), sizeof(SequenceId));
        seqId = ntohll(seqId);
        offset += sizeof(SequenceId);

        memcpy(&checksum, data.data() + offset, sizeof(Checksum));
        checksum = ntohs(checksum);
        offset += sizeof(Checksum);

        payload.resize(data.size() - offset);
        memcpy(payload.data(), data.data() + offset, payload.size());
    }

    std::shared_ptr<Body> BodyFactory::createBody(Keyword keyword, std::vector<Byte> data) {
        switch (keyword) {
        case LIST_FILES:
            return std::make_shared<Body>(data);
        case FILES:
            return std::make_shared<Files>(data);
        case DOWNLOAD:
            return std::make_shared<Download>(data);
        case UPLOAD:
            return std::make_shared<Upload>(data);
        case RESEND:
            return std::make_shared<Resend>(data);
        case ACK:
            return std::make_shared<Body>();
        case ABORT:
            return std::make_shared<Body>();
        case PACKET:
            return std::make_shared<Packet>(data);
        default:
            throw std::runtime_error("fup::BodyFactory: Invalid keyword");
        }
    }

    std::shared_ptr<PacketQueue> QueueMap::operator[](const ConnectionId& id) {
        std::lock_guard<std::mutex> lock(*mutex);
        return (*qMap)[id];
    }

    void QueueMap::Remove(const ConnectionId& id) {
        std::lock_guard<std::mutex> lock(*mutex);
        qMap->erase(id);
    }

    void PacketQueue::Push(const Message& packet) {
        std::lock_guard<std::mutex> lock(*packetQueueMutex);
        packetQueue->push(packet);
    }

    Message PacketQueue::Pop() {
        std::lock_guard<std::mutex> lock(*packetQueueMutex);
        if (packetQueue->empty()) {
            throw std::runtime_error("Packet queue is empty");
        }
        Message message = packetQueue->front();
        packetQueue->pop();
        return message;
    }

    bool PacketQueue::IsEmpty() const {
        std::lock_guard<std::mutex> lock(*packetQueueMutex);
        return packetQueue->empty();
    }
}