#include <vector>
#include <cstdint>
#include <stdexcept>
#include "ReaderThread.h"
#include "ChunkListSource.h"
#include "ChunkQueue.h"
#include "DataSource.h"
#include <gtest/gtest.h>

// Case 1: Pop a ChunkListSource until False, error() should be empty & number of chunks = actual
TEST(readerThreadTest, PopUntilFalse){ 
    size_t counter = 0;
    std::vector<uint8_t> outChunk;
    std::vector<std::vector<uint8_t>> chunks{ 
        {0x11, 0x22, 0x33},
        {0x44, 0x55, 0x66}
    };
    ChunkListSource chunkList(chunks);
    ChunkQueue que;

    ReaderThread readerThread(chunkList, que);
    while(que.pop(outChunk)){
        counter++;
    }
    EXPECT_EQ(counter, 2);

    EXPECT_EQ(readerThread.error(), "");
}

// A mock object that simulates a hardware failure of unplugging a USB cable
struct FakeSource : public DataSource {
    std::vector<uint8_t> readChunk() override {
        // Simulate the USB cable being disconnected
        throw std::runtime_error("USB unplugged");
    }

    bool isFinished() const override{
        return false;
    };

};

// Test mock DataSource and check if error messages are equal
TEST(readerThreadTest, ThrowsErrorWhenUnplugged){ 
    std::vector<uint8_t> outChunk;
    FakeSource mockSource;
    ChunkQueue que;

    ReaderThread readerThread(mockSource, que);
    while(que.pop(outChunk)){}

    readerThread.stop();
    EXPECT_EQ(readerThread.error(), "USB unplugged");
}