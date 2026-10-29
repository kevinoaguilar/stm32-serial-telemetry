#pragma once

#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include "DataSource.h"

class RecordingSource : public DataSource{
public:
    RecordingSource(std::unique_ptr<DataSource> src, const std::string& path);

    std::vector<uint8_t> readChunk() override;

    bool isFinished() const override;

private:
    std::unique_ptr<DataSource> inner;
    std::ofstream out;
};