#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <windows.h>
#include <vector>
#include "SerialPort.h"


SerialPort::SerialPort(const std::string& portName, const DWORD bitRate) : portName(portName), baudRate(bitRate) {
    if (portName.empty()){
        throw std::runtime_error("COM Port " + portName + " could not be open");
    }

    std::string fullPath = "\\\\.\\" + portName;
    hport = CreateFileA(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hport == INVALID_HANDLE_VALUE){
        std::string error = std::to_string(GetLastError());
        throw std::runtime_error(fullPath + " could not be open. Error code: " + error);
    }

    DCB dcb {};
    dcb.DCBlength = sizeof(dcb); // Tells Windows the size of the data structure to find out what Windows API version I am targeting
    if (!GetCommState(hport, &dcb)) {
        std::string error = std::to_string(GetLastError());
        CloseHandle(hport);
        throw std::runtime_error("Could not fetch the configure settings on " + portName + ". Error Code: " + error);
    }   // Windows fills the 28 configuration fields on dcb with the handle configurations

    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hport, &dcb)) {
        std::string error = std::to_string(GetLastError());
        CloseHandle(hport); // MUST close handle before the throw, as throwing exception skips destructor causing handle to be locked
        throw std::runtime_error("Could not set LINE configuration settings on " + portName + ". Error Code: " + error);
    }

    COMMTIMEOUTS t{};
    t.ReadIntervalTimeout           = MAXDWORD;
    t.ReadTotalTimeoutMultiplier    = MAXDWORD;
    t.ReadTotalTimeoutConstant      = 1000;     // 1000 milliseconds or 1 second

    if (!SetCommTimeouts(hport, &t)) {
        std::string error = std::to_string(GetLastError());
            CloseHandle(hport); 
            throw std::runtime_error("Could not set TIMEOUT configuration settings on " + portName + ". Error Code: " + error);
    }


    std::cout << "VALID     " << fullPath << "  ->  " << hport << "\n";
}

SerialPort::~SerialPort() {
    CloseHandle(hport);
    std::cout << portName << " port is now closed.\n";
}

std::vector<uint8_t> SerialPort::readChunk() {
    std::vector<uint8_t> bucket(256);
    DWORD bytesRead = 0;

    BOOL ok = ReadFile(
        hport,                  // port of the handle
        bucket.data(),          // raw memory address where Windows will store bytes in
        (DWORD)bucket.size(),   // max capacity (256)
        &bytesRead,              // out-parameter to store number of bytes it read
        nullptr                // async I/O
    );
    if (!ok) {
        std::string error = std::to_string(GetLastError());
        throw std::runtime_error("Could not read incoming data from " + portName + ". Error code: " + error);
    }

    bucket.resize(bytesRead);
    return bucket;
}

bool SerialPort::isFinished() const {
    return false;
}