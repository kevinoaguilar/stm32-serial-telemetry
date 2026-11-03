#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// input must be shorter than 254 bytes
size_t cobsEncode (const uint8_t* in, size_t len, uint8_t* out);

// 'out' is always cleared
bool cobsDecode (const uint8_t* in, size_t len, std::vector<uint8_t>& out);