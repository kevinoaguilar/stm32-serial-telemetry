#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "ChunkQueue.h"

void ChunkQueue::push(std::vector<uint8_t> chunk)
{
    if (chunk.empty())
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m);
        chunks.push(std::move(chunk));
    }

    cv.notify_one();
}

bool ChunkQueue::pop(std::vector<uint8_t> &out)
{
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [this]
            { return closed || !chunks.empty(); });

    if (chunks.empty())
    {
        return false;
    }

    out = std::move(chunks.front());
    chunks.pop();

    return true;
}

void ChunkQueue::close()
{
    {
        std::lock_guard<std::mutex> lock(m);
        closed = true;
    }

    cv.notify_all();
}