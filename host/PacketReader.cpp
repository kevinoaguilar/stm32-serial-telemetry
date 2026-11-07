#include "PacketReader.h"

std::vector<ParsedPacket> PacketReader::feed(const std::vector<uint8_t>& chunk) {
    std::vector<ParsedPacket> packets;
    pending.insert(pending.end(), chunk.begin(), chunk.end());

    while (!pending.empty()) {
        size_t index = 0;
        bool zeroFound = false;
        for (size_t i = 0; i < pending.size(); i++){
            if (pending[i] == 0x00){
                index = i;
                zeroFound = true;
                break;
            } 
        }

        if (!zeroFound) {
            break;
        }

        std::vector<uint8_t> frame(pending.begin(), pending.begin() + index);
        pending.erase(pending.begin(), pending.begin() + index + 1);

        if (frame.empty()){
            continue;
        }

        ParsedPacket packet = parseFrame(frame);
        packets.push_back(packet);
    }
    
    return packets;
}
