#pragma once
#include "Parser.h"
#include <cstdint>
#include <vector>

class PacketReader {
public:
    std::vector<ParsedPacket> feed(const std::vector<uint8_t>& chunk);

    const std::vector<uint8_t>& getPending() const { return pending; }

private:
    std::vector<uint8_t> pending;
};

