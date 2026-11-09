/*
 * cobs.c
 *
 *  Created on: Nov 6, 2026
 *      Author: koagu
 */

#include <stdint.h>
#include <stddef.h>

#include "cobs.h"

size_t cobsEncode(const uint8_t* in, size_t len, uint8_t* out){
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
