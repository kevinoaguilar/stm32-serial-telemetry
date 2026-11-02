#include <cstdint>
#include "Crc16.h"

uint16_t crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = crc ^ (uint16_t)(data[i] << 8);
        for (size_t o = 0; o < 8 ; o++) {
            if ((crc & 0x8000) != 0) {
                crc = (uint16_t)(crc << 1);
                crc = (uint16_t)(crc ^ 0x1021);
            }
            else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }
    return crc;
}
