#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>
#include "DataSource.h"

class SerialPort : public DataSource {
public:
    SerialPort(const std::string& portName, const DWORD bitRate);
    ~SerialPort();
        
    std::vector<uint8_t> readChunk();
    bool isFinished() const override;

    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    std::string portname() const { return portName; }

private:
    std::string portName;
    HANDLE hport;
    DWORD baudRate;
};