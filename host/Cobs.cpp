#include <cstdint>
#include <cstddef>
#include <vector>

#include "Cobs.h"


size_t cobsEncode (const uint8_t* in, size_t len, uint8_t* out){
    size_t codeIndex = 0;   // Index in 'out' marking where current block's code byte will go
    size_t write = 1;       // Tracks the index in 'out' where actual data bytes should be copied into
    uint8_t code = 1;        // Counter, tracking number of bytes in current block, incl. code byte

    for (size_t i = 0; i < len; i++) {
        if (in[i] == 0x00) {
            out[codeIndex] = code;
            codeIndex = write;

            write++;
            code = 1;
        }
        else {
            out[write] = in[i];
            write++;
            code++;
        }
    }
    out[codeIndex] = code;

    return write;
}

bool cobsDecode (const uint8_t* in, size_t len, std::vector<uint8_t>& out){
    out.clear();
    size_t i = 0;   // Reading index for 'in'

    while (i < len){
        uint8_t code = in[i];   

        // Validation checks
        if (code == 0 || i + code > len) { 
            return false;
        }

        // Copy the actual data bytes
        for (size_t j = 1; j < code; j++) { 
            out.push_back(in[i + j]);
        }

        // Update reading index to the first byte of the next block
        i += code;

        // Restore the 0x00 that was removed originally back into the output data
        if ( i < len ) {
            out.push_back(0x00);
        }
    }
    return true;
}