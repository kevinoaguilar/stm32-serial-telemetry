#pragma once

#include "DataSource.h"

class ChunkListSource : public DataSource {
    public:
    ChunkListSource( const std::vector<std::vector<uint8_t>>& chunks ) : next(0) , sourceChunks(chunks) {} 

    std::vector<uint8_t> readChunk() override;
    bool isFinished() const override;

    private:
    size_t next;
    std::vector<std::vector<uint8_t>> sourceChunks;
};