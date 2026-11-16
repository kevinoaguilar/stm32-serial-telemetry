cmake_minimum_required(VERSION 3.16)
project(telemetry LANGUAGES CXX)

# Enforce strict C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The Core Library (No main(), No windows.h)
add_library(telemetry_core STATIC
    Pipeline.cpp
    ReaderThread.cpp
    ChunkQueue.cpp
    ChunkListSource.cpp
    RecordingSource.cpp
    FileSource.cpp
    PacketReader.cpp
    Parser.cpp
    Cobs.cpp
    Crc16.cpp
)

# Warnings to the library
target_compile_options(telemetry_core PRIVATE -Wall -Wextra)

# Linking multithreading libraries PUBLICLY
find_package(Threads REQUIRED)
target_link_libraries(telemetry_core PUBLIC Threads::Threads)

# Windows ONLY Executable
if(WIN32)
    add_executable(telemetry
        telemetry.cpp
        SerialPort.cpp
    )

    # Applying warnings to Windoes exe
    target_compile_options(telemetry PRIVATE -Wall -Wextra)

    # Linking library to executable
    target_link_libraries(telemetry PRIVATE telemetry_core)
endif()