#include "Parser.h"
#include <cstdint>
#include <vector>
#include <gtest/gtest.h>

// Verifying if the parseFrame function is valid with actual matching payload
TEST(ParserTest, PayloadMatch){
    std::vector<uint8_t> parseTest = {0x06, 0x11, 0x22, 0x05, 0x88, 0xEE};
    ParsedPacket parsedp = parseFrame(parseTest);
    
    // Verifying if payload & actual payload match
    std::vector<uint8_t> expected = {0x11, 0x22, 0x05};
    EXPECT_EQ(parsedp.payload, expected);
}

// Test with same frame as 'PayloadMatch' with a single byte changed, returns Invalid
TEST(ParserTest, RejectsChangedBytes){
    std::vector<uint8_t> parseTest = {0x06, 0x11, 0x33, 0x05, 0x88, 0xEE};
    ParsedPacket parsedp = parseFrame(parseTest);
    
    // Verifying if CRC calculation differs from a changed single byte
    EXPECT_EQ(parsedp.valid, false);
}

// A one byte frame would return invalid as size must be greater than 2, no crash
TEST(ParserTest, OneByteFrameButNoCrash){
    std::vector<uint8_t> parseTest = {0x06, 0x11};
    ParsedPacket parsedp = parseFrame(parseTest);
    
    // Frame must be at least 2 bytes so the decoded frame can calcaulates its CRC
    EXPECT_EQ(parsedp.valid, false);
}