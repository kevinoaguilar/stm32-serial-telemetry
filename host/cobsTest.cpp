#include <cstdint>
#include <vector>
#include <iostream>
#include "Cobs.h"

int main() {
    std::vector<std::vector<uint8_t>> cobsTest = {
        {0x11, 0x22, 0x00, 0x33},
        {0x11, 0x22, 0x33},
        {0x00},
        {0x00, 0x00},
        {}
    };

    std::cout << std::hex;

    for (const std::vector<uint8_t>& b : cobsTest) {
        // encode: the lenOfFrame slots of frame are filled
        uint8_t frame[5];
        size_t lenOfFrame = cobsEncode(b.data(), b.size(), frame);

        std::cout << "encoded: ";
        for (size_t i = 0; i < lenOfFrame; i++) {
            std::cout << (int)frame[i] << " ";
        }
        std::cout << "\n";

        // decode what encode produced, then compare with the original
        std::vector<uint8_t> decodedFrame;
        bool check = cobsDecode(frame, lenOfFrame, decodedFrame);

        std::cout << "decoded: ";
        for (uint8_t d : decodedFrame) {
            std::cout << (int)d << " ";
        }
        std::cout << "\n";

        if (check && decodedFrame == b) {
            std::cout << "OK\n\n";
        }
        else {
            std::cout << "FAIL\n\n";
        }
    }

    // a broken frame: the code 05 says 4 data bytes follow, but only 2 do
    std::vector<uint8_t> broken = {0x05, 0x11, 0x22};
    std::vector<uint8_t> brokenOut;
    bool brokenCheck = cobsDecode(broken.data(), broken.size(), brokenOut);
    std::cout << "broken frame decodes: " << (brokenCheck ? "true" : "false") << "\n";
}