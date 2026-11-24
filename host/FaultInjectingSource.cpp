#include <memory>
#include <cstdint>
#include <vector>
#include <utility>
#include "DataSource.h"
#include "FaultInjectingSource.h"

FaultInjectingSource::FaultInjectingSource(std::unique_ptr<DataSource> src, size_t step)
: inner(std::move(src)), flipEvery(step), counter(0) {}

std::vector<uint8_t> FaultInjectingSource::readChunk() {
    std::vector<uint8_t> corrupted = inner->readChunk();
    if (flipEvery == 0){
        return corrupted;
    }

  
    for(uint8_t& byte : corrupted){
        counter++;
        if (counter % flipEvery == 0){
            byte ^= 1;
        }
    }
    return corrupted;
}

bool FaultInjectingSource::isFinished() const{
    return inner->isFinished();
}