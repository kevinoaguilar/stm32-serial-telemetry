#include <cstdint>
#include <iostream>
#include <vector>
#include <numeric>
#include "Crc16.h"

int main() {
    std::vector<std::vector<uint8_t>> crcTest = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9'},
        {0x11, 0x22, 0x05},
        {0x22, 0x11, 0x05},
        {}
    };

    for (const std::vector<uint8_t>& b : crcTest) {
        uint8_t sum = std::accumulate(b.begin(), b.end(), 0);
        std::cout << std::hex << "crc = " << crc16(b.data(), b.size()) << "     "
                  << "sum = " << (int)sum << "\n" ;
        std::cout << "\n";
    }
}