#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <fstream>
#include "DataSource.h"
#include "FileSource.h"

FileSource::FileSource(const std::string& path, size_t srcChunkSize)
: in(path, std::ios::binary), chunkSize(srcChunkSize)    {
    if (path.empty()){
        throw std::runtime_error("Path cannot be empty.");
    }
    
    if (!in.is_open()) {
        throw std::runtime_error("Could not open file, check path on " + path);
    }
}


std::vector<uint8_t> FileSource::readChunk(){
    std::vector<uint8_t> buf(chunkSize);

    in.read(reinterpret_cast<char*>(buf.data()), chunkSize);
    buf.resize(static_cast<size_t>(in.gcount()));

    return buf;
}

bool FileSource::isFinished() const {
    return in.eof();
}