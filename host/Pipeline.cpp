#include "DataSource.h"
#include "PacketReader.h"
#include <iostream>
#include "Pipeline.h"

void runPipeline(DataSource& source, PacketReader& reader, int maxReads){
    int reads = 0;
    int currentChunk = 0;

    while (!source.isFinished() && reads < maxReads) {
        std::vector<uint8_t> bytes(source.readChunk());
            ++reads;

        if (bytes.empty()) {
            continue;
        }

        currentChunk++;
        std::vector<ParsedPacket> packets = reader.feed(bytes);
        std::cout << "Chunks received: " << currentChunk << "\nPackets created: " << packets.size() << "\n";
          for (const ParsedPacket& packet : packets) {
                if (packet.valid) {
                    std::cout << " VALID    -> " << "   Payload size : " << packet.payload.size() << "\n";
                }
                else {
                    std::cout << "INVALID\n";
                } 
                for (uint8_t b : packet.payload) {
                    std::cout << std::hex << (int)b << " " << std::dec;
                }
                std::cout << "\n";
          }
    }
}

