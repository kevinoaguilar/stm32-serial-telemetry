#include <cstdint>
#include <vector>

#include "Cobs.h"

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