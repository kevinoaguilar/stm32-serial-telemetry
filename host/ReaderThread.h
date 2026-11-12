#pragma once

#include <string>
#include <atomic>
#include <thread>
#include "DataSource.h"
#include "ChunkQueue.h"

class ReaderThread {
public:
    ReaderThread(DataSource& source, ChunkQueue& queue);
    ~ReaderThread();                 // calls stop()
    void stop();                     // ask the thread to stop, then join it (safe to call twice)
    const std::string& error() const;   // empty if the reader stopped normally
    
private:
    void run();                      // the loop the thread runs
    DataSource& source;
    ChunkQueue& queue;
    std::atomic<bool> stopRequested{false};
    std::string errorMessage;
    std::thread worker;              // keep this LAST
};