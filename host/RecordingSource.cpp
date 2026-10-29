#include <memory>
#include <stdexcept>
#include <fstream>
#include <utility>
#include "DataSource.h"
#include "RecordingSource.h"
  
RecordingSource::RecordingSource(std::unique_ptr<DataSource> src, const std::string& path) 
: inner(std::move(src)),  out(path, std::ios::binary) {
    if (path.empty()){
        throw std::runtime_error("Path name cannot be empty.");
    }

    if (!out.is_open()) {
        throw std::runtime_error("Could not write content into " + path);
    }
}

std::vector<uint8_t> RecordingSource::readChunk() {
    std::vector<uint8_t> chunk = inner->readChunk();
    if (!chunk.empty()){
        out.write(reinterpret_cast<const char*>(chunk.data()),
                  static_cast<std::streamsize>(chunk.size()));
        out.flush();
    }
    return chunk;
}

bool RecordingSource::isFinished() const {
    return inner->isFinished();
}

