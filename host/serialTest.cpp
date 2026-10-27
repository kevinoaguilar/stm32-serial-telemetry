#include "SerialPort.h"
#include "Pipeline.h"
#include <iostream>

int main() {
    try {
        SerialPort port("COM5", 115200);
        std::cout << "opened\n";
        
        PacketReader reader;
        runPipeline(port, reader, 10);   

    } catch (const std::exception& e) {
            std::cout << "error: " << e.what() << "\n";
    }
    std::cout << "end of main\n";
}