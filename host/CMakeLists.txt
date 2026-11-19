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

# Downloading GoogleTest source code dependency
include(FetchContent)
FetchContent_Declare(
    googletest 
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Turn on Testing
enable_testing()

# GoogleTest Executable ONLY for running tests
add_executable(unit_tests 
    tests/crcTest.cpp
    tests/cobsTest.cpp
    tests/parserTest.cpp
    tests/packetReaderTest.cpp
    tests/chunkQueueTest.cpp
    tests/readerThreadTest.cpp
)

# Linking Testing Executable
target_link_libraries(unit_tests PRIVATE telemetry_core GTest::gtest_main)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)

# Allow tests/ access to fetch from hosts/
target_include_directories(telemetry_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
