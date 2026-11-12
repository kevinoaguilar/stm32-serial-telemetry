#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstdint>

class ChunkQueue {
public:
    void push(std::vector<uint8_t> chunk);   // add a chunk, wake a waiter
    bool pop(std::vector<uint8_t>& out);     // wait for a chunk; false = closed and nothing left
    void close();                            // no more chunks are coming
    
private:
    std::mutex m;
    std::condition_variable cv;
    std::queue<std::vector<uint8_t>> chunks;
    bool closed = false;
};