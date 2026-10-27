#include "PacketReader.h"

std::vector<ParsedPacket> PacketReader::feed(const std::vector<uint8_t>& chunk) {
    std::vector<ParsedPacket> packets;
    pending.insert(pending.end(), chunk.begin(), chunk.end());

    while (!pending.empty()) {
        while (!pending.empty() && pending.front() != 0xAA) {
            pending.erase(pending.begin());
        }

        if (pending.size() < 2){
            break;
        }   

        if (pending.size() < (size_t)pending[1] + 3){
            break;
        }

        uint8_t payloadLength = pending[1];
        size_t totalPacketSize = payloadLength + 3;
        std::vector<uint8_t> parse(pending.begin(), pending.begin() + totalPacketSize);
        ParsedPacket packet = parsePacket(parse);
        if (!packet.valid){
            pending.erase(pending.begin());
        }
        else{
            pending.erase(pending.begin(), pending.begin() + totalPacketSize);
        }
        packets.push_back(packet);

    }
    return packets;

}
