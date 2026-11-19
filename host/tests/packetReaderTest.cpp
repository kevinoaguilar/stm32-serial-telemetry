#include "../PacketReader.h"
#include <cstdint>
#include <vector>
#include <gtest/gtest.h>

// Case 1: Split frame across two feeds, initially 0 packets, then 1
TEST(PacketReaderTest, SplitReader){
    PacketReader reader;

    // First half of frame (No Packets)
    std::vector<uint8_t> chunk1 = {0x11, 0x22};
    auto packets1 = reader.feed(chunk1);

    // Reader should buffer it, and return empty 
    EXPECT_EQ(packets1.size(), 0);

    // Second half of frame (A complete packet, includes delimeter '0x00)
    std::vector<uint8_t> chunk2 = {0x33, 0x00};
    auto packets2 = reader.feed(chunk2);

    // Reader concatenates both, creating one complete packet
    EXPECT_EQ(packets2.size(), 1);
}

// Case 2: Two frames in a single chunk, creating two packets
TEST(PacketReaderTest, TwoFramesInOneChunk){
    PacketReader reader;
    std::vector<uint8_t> twoFrame = {0x11, 0x22, 0x33, 0x00, 0x44, 0x55, 0x66, 0x00};
    auto packets = reader.feed(twoFrame);

    // Must return 2 complete packets from a single chunk
    EXPECT_EQ(packets.size(), 2);
}

// Chunk containing only delimeter (0x00), should return no packets
TEST(PacketReaderTest, LeadingZeroNoPacket){
    PacketReader reader;

    std::vector<uint8_t> leadingZero = {0x00};
    auto packets = reader.feed(leadingZero);

    EXPECT_EQ(packets.size(), 0);
}