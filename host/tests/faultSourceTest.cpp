#include "FaultInjectingSource.h"
#include "ChunkListSource.h"
#include "PacketReader.h"
#include <utility>
#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <memory>

TEST(faultSourceTest, FaultyChunksInjectingEvery3Bytes)
{
    size_t valid = 0;
    size_t invalid = 0;
    PacketReader reader;
    std::vector<std::vector<uint8_t>> chunks = {
        {0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00}};
    std::unique_ptr<DataSource> source = std::make_unique<ChunkListSource>(std::move(chunks));
    FaultInjectingSource faultSource(std::move(source), 3);

    while (!faultSource.isFinished())
    {
        std::vector<uint8_t> corruptedBytes = faultSource.readChunk();
        std::vector<ParsedPacket> packets = reader.feed(corruptedBytes);
        for(const ParsedPacket& packet : packets){
            if(packet.valid){
                valid++;
            }
            else{
                invalid++;
            }
        }
    }

    EXPECT_EQ(valid, 0);
    EXPECT_EQ(invalid , 3);
}

TEST(faultSourceTest, FaultyChunksInjectingEvery0Byte)
{
    size_t valid = 0;
    size_t invalid = 0;
    PacketReader reader;
    std::vector<std::vector<uint8_t>> chunks = {
        {0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00}};
    std::unique_ptr<DataSource> source = std::make_unique<ChunkListSource>(std::move(chunks));
    FaultInjectingSource faultSource(std::move(source), 0);

    while (!faultSource.isFinished())
    {
        std::vector<uint8_t> corruptedBytes = faultSource.readChunk();
        std::vector<ParsedPacket> packets = reader.feed(corruptedBytes);
        for(const ParsedPacket& packet : packets){
            if(packet.valid){
                valid++;
            }
            else{
                invalid++;
            }
        }
    }

    EXPECT_EQ(valid, 4);
    EXPECT_EQ(invalid , 0);
}

TEST(faultSourceTest, FaultyChunksInjectingEvery7Bytes)
{
    size_t valid = 0;
    size_t invalid = 0;
    PacketReader reader;
    std::vector<std::vector<uint8_t>> chunks = {
        {0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00,
         0x06, 0x11, 0x22, 0x05, 0x88, 0xEE, 0x00}};
    std::unique_ptr<DataSource> source = std::make_unique<ChunkListSource>(std::move(chunks));
    FaultInjectingSource faultSource(std::move(source), 7);

    while (!faultSource.isFinished())
    {
        std::vector<uint8_t> corruptedBytes = faultSource.readChunk();
        std::vector<ParsedPacket> packets = reader.feed(corruptedBytes);
        for(const ParsedPacket& packet : packets){
            if(packet.valid){
                valid++;
            }
            else{
                invalid++;
            }
        }
    }
    size_t size = reader.getPending().size();

    EXPECT_EQ(size, 28);
    EXPECT_EQ(valid, 0);
    EXPECT_EQ(invalid , 0);
}