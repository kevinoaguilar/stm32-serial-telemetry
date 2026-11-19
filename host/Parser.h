#pragma once

#include <cstdint>
#include <vector>

struct ParsedPacket {
    bool valid;
    uint8_t length;
    std::vector<uint8_t> payload;
};

ParsedPacket parseFrame(const std::vector<uint8_t>& frame);

