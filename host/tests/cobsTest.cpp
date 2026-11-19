#include <cstdint>
#include <vector>
#include "Cobs.h"
#include <gtest/gtest.h>



// Case 1: Encodes bytes, checks newly encoded bytes against known encoded bytes
TEST(CobsTest, EncodesKnownBytes) {
    std::vector<uint8_t> cobsTest = {0x11, 0x22, 0x00, 0x33};

        // encode: the lenOfFrame slots of frame are filled
        uint8_t frame[5];
        size_t lenOfFrame = cobsEncode(cobsTest.data(), cobsTest.size(), frame);
        std::vector<uint8_t> encoded(frame, frame + lenOfFrame);

        // Checking if encoded frame is correct
        std::vector<uint8_t> expected = {0x03, 0x11, 0x22, 0x02, 0x33};
        EXPECT_EQ(encoded, expected) << "Case 1  ->  FAILED : Encoded frame mismatched against actual";
}



// Case 2: Iterates over 5 cases, determines which have failed, to output error message
TEST(CobsTest, RoundTrip){
        std::vector<uint8_t> cobsTest = {0x11, 0x22, 0x00, 0x33};

        uint8_t frame[5];
        size_t lenOfFrame = cobsEncode(cobsTest.data(), cobsTest.size(), frame);
        std::vector<uint8_t> encoded(frame, frame + lenOfFrame);

        std::vector<uint8_t> expected = {0x03, 0x11, 0x22, 0x02, 0x33};
        EXPECT_EQ(encoded, expected) << "Case 2  ->  FAILED : Encoded frame mismatched against actual";


        // decode what encode produced, then compare with the original
        std::vector<uint8_t> decodedFrame;
        bool check = cobsDecode(frame, lenOfFrame, decodedFrame);

        // Verify if frame decoded correctly, if not immediately stop testing
        ASSERT_TRUE(check) << "Case 2  ->  FAILED : Decoding failed on the encoded frame";

        // Checking that both decoded & original cobsTest frame match
        EXPECT_EQ(cobsTest, decodedFrame) << "Case 2  ->  FAILED : Decoded frame mismatched against actual";
}



// Case 3: Verifies that decoding a broken encoded frame returns False
TEST(CobsTest, RejectsBrokenFrame){

    // Code byte (0x05) on encoded bytes point to a 0x00 that is Out of Range
    uint8_t brokenFrame[3] = {0x05, 0x11, 0x22};
    std::vector<uint8_t> decodedFrame;

    // Must return False as first byte causes an out-of-bounds access
    bool check = cobsDecode(brokenFrame, 3, decodedFrame);

    EXPECT_FALSE(check);
}