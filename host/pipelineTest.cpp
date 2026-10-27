#include "Pipeline.h"
#include "ChunkListSource.h"

int main() {
    ChunkListSource source({
    {0xAA, 0x03, 0x10},
    {},
    {0x20, 0x30, 0x60, 0xAA, 0x00},
    {0x00}
    });
    
    PacketReader reader;
    runPipeline(source, reader, 10);
}
