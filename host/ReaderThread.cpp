#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <exception>
#include "DataSource.h"
#include "ChunkQueue.h"
#include "ReaderThread.h"

ReaderThread::ReaderThread(DataSource &source, ChunkQueue &queue)
    : source(source), queue(queue), worker(&ReaderThread::run, this)
{
}

ReaderThread::~ReaderThread()
{
    stop();
}

void ReaderThread::stop()
{
    stopRequested = true;
    if (worker.joinable())
    {
        worker.join();
    }
}

const std::string &ReaderThread::error() const
{
    return errorMessage;
}

void ReaderThread::run()
{
    try
    {
        while (!stopRequested && !source.isFinished())
        {
            std::vector<uint8_t> chunk = source.readChunk();
            if (!chunk.empty())
            {
                queue.push(std::move(chunk));
            }
        }
    }
    catch (const std::exception &e)
    {
        errorMessage = e.what();
    }
    queue.close();
}