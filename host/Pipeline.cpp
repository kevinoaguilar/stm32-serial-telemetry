#include <iostream>
#include <vector>
#include "DataSource.h"
#include "PacketReader.h"
#include "Pipeline.h"
#include "ReaderThread.h"
#include "ChunkQueue.h"

void runPipeline(DataSource &source, PacketReader &reader, int maxReads)
{
    int reads = 0;
    ChunkQueue queue;
    ReaderThread readerthread(source, queue);
    std::vector<uint8_t> chunks;

    while (queue.pop(chunks))
    {
        ++reads;
        
        std::vector<ParsedPacket> packets = reader.feed(chunks);
        std::cout << "Chunks received: " << reads << "\nPackets created: " << packets.size() << "\n";
        for (const ParsedPacket &packet : packets)
        {
            if (packet.valid)
            {
                std::cout << " VALID    -> " << "   Payload size : " << packet.payload.size() << "\n";
            }
            else
            {
                std::cout << "INVALID\n";
            }
            for (uint8_t b : packet.payload)
            {
                std::cout << std::hex << static_cast<int>(b) << " " << std::dec;
            }
            std::cout << "\n";
        }
        if (reads >= maxReads && maxReads > 0)
        {
            break;
        }
    }
    readerthread.stop();

    if (!readerthread.error().empty())
    {
        std::cerr << "Reader stopped: " << readerthread.error() << "\n";
    }
}
