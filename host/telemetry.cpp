#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <exception>
#include "DataSource.h"
#include "SerialPort.h"
#include "Pipeline.h"
#include "PacketReader.h"
#include "ChunkListSource.h"
#include "RecordingSource.h"
#include "FileSource.h"


std::unique_ptr<DataSource> makeSource(const std::string& kind, const std::string& path){
    if (kind == "serial") {
        return std::make_unique<SerialPort>("COM5", 115200);
    }
    else if (kind == "test") {
        std::vector<std::vector<uint8_t>> chunks({
        {0xAA, 0x03, 0x10},
        {},
        {0x20, 0x30, 0x60, 0xAA, 0x00},
        {0x00}
        });

        return std::make_unique<ChunkListSource>(chunks);

    }
    else if (kind == "record") {
        std::unique_ptr<DataSource> sourceRecord(std::make_unique<SerialPort>("COM5", 115200));
        return std::make_unique<RecordingSource>(std::move(sourceRecord), path);
    }
    else if (kind == "replay") {
        return std::make_unique<FileSource>(path, 4);
    }
    else {
        throw std::runtime_error("unknown source: " + kind);
    }
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cout << "Usage: telemetry serial | test | record <file> | replay <file> \n";
        return 1;
    }

    std::string kind = argv[1];
    std::string path = "";
    if (argc >= 3){
        path = argv[2];
    }

    try {
        std::unique_ptr<DataSource> source = makeSource(kind, path);
        PacketReader reader;
        runPipeline(*source, reader, 10);
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}