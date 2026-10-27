#pragma once 

#include <vector>
#include <cstdint>

class DataSource {
    public:
    // reading bytes that came thorugh the board
    virtual std::vector<uint8_t> readChunk() = 0;

    // instead of terminating on a empty chunk, 
    // loop while SourcePort is not empty, and skip empty chunks
    virtual bool isFinished() const = 0;

    // destructor s
    virtual ~DataSource() = default;

};