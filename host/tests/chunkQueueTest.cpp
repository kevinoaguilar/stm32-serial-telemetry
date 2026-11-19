#include "ChunkQueue.h"
#include <vector>
#include <cstdint>
#include <gtest/gtest.h>

// Case 1: Two chunks pushed into queue, close(), then pop each
// Returns both in order, then False for an empty queue
TEST(ChunkQueueTest, PopReturnsInOrder){
    std::vector<uint8_t> chunk1 = {0x11, 0x22, 0x33};
    std::vector<uint8_t> chunk2 = {0x44, 0x55, 0x66};

    ChunkQueue que;
    que.push(chunk1);
    que.push(chunk2);
    que.close();

    std::vector<uint8_t> outChunk;

    // First pop must return true and match chunk1
    bool check1 = que.pop(outChunk);
    EXPECT_TRUE(check1);
    EXPECT_EQ(chunk1, outChunk);

    // Second pop, needs to return true and match chunk2
    bool check2 = que.pop(outChunk);
    EXPECT_TRUE(check2);
    EXPECT_EQ(chunk2, outChunk);

    // Third pop on empty queue, must return false
    bool check3 = que.pop(outChunk);
    EXPECT_FALSE(check3);
}

// Case 2: An early close() and empty queue, returns False
TEST(ChunkQueueTest, ClosedEmptyReturnsFalse){
    ChunkQueue que;
    que.close();

    // Pop on a closed & empty queue, must return false
    std::vector<uint8_t> outChunk;
    bool check = que.pop(outChunk);
    
    EXPECT_FALSE(check);
}