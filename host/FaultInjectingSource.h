#pragma once

#include <memory>
#include <stdint.h>
#include <vector>
#include "DataSource.h"

class FaultInjectingSource : public DataSource{
public:
    FaultInjectingSource(std::unique_ptr<DataSource> src, size_t step);

    std::vector<uint8_t> readChunk() override;

    bool isFinished() const override;

private:
    std::unique_ptr<DataSource> inner;
    size_t flipEvery;
    size_t counter;
};