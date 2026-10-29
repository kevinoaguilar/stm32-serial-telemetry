#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include "DataSource.h"


class FileSource : public DataSource {
public:
    FileSource(const std::string& path, size_t srcChunkSize);

    std::vector<uint8_t> readChunk() override;
    bool isFinished() const override;

private:
    std::ifstream in;
    size_t chunkSize;
};

