#include <cstdint>
#include <vector>
#include "Crc16.h"
#include <gtest/gtest.h>

// Case 1: Crc16 calculation on uint8_t vector must return '0x29B1'
TEST(CrcTest, CheckValue) {
    std::vector<uint8_t> crcTest = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint16_t result = crc16(crcTest.data(), crcTest.size());

    EXPECT_EQ(result, 0x29B1);
}

// Case 2: Crc16 calculation on an empty vector must return '0xFFFF'
TEST(CrcTest, EmptyInput) {
    std::vector<uint8_t> crcTest;
    uint16_t result = crc16(crcTest.data(), crcTest.size());

    EXPECT_EQ(result, 0xFFFF);
}