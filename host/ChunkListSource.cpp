#include "ChunkListSource.h"

// Does not need 'const' as the 'next' variable will be manipulated
// Violating the 'read only' rule 'const' has.
std::vector<uint8_t> ChunkListSource::readChunk() {
        if (next >= sourceChunks.size()){
            return {};
        }
        std::vector<uint8_t> currentChunk(sourceChunks[next]);
        next++;
        return currentChunk;
}

bool ChunkListSource::isFinished() const{
    return (next >= sourceChunks.size());
}