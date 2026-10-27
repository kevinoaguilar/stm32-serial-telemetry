#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct ParsedPacket {
    bool valid;
    uint8_t length;
    std::vector<uint8_t> payload;
};

uint8_t computeChecksum(const std::vector<uint8_t>& payload);

ParsedPacket parsePacket(const std::vector<uint8_t>& buf);

void printResult(const std::string& name, const ParsedPacket& p);
