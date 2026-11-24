cmake_minimum_required(VERSION 3.16)
project(telemetry LANGUAGES CXX)

# Enforce strict C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Switch for 'TSan,' checking for any dtaat races through every memory read/write
option(TELEMETRY_TSAN "Build with ThreadSanitizer" OFF)

# When ON, compiles everyting with TSan
if(TELEMETRY_TSAN)
    add_compile_options(-fsanitize=thread -g -O1)
    add_link_options(-fsanitize=thread)
endif()

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
    FaultInjectingSource.cpp
)

# Warnings to the library
target_compile_options(telemetry_core PRIVATE -Wall -Wextra)

# Declaring a switch for '-Werror' to prohibit warnings from becoming hard compile errors on testing code, default to OFF
option(TELEMETRY_WERROR "Treat warnings as errors" OFF)


# CI will always turn on '-Werror' to catch any warnings
if(TELEMETRY_WERROR)
    target_compile_options(telemetry_core PRIVATE -Werror)
endif()

# Linking multithreading libraries PUBLICLY
find_package(Threads REQUIRED)
target_link_libraries(telemetry_core PUBLIC Threads::Threads)

# Windows ONLY Executable
if(WIN32)
    add_executable(telemetry
        telemetry.cpp
        SerialPort.cpp
    )

    # Applying warnings to Windows exe
    target_compile_options(telemetry PRIVATE -Wall -Wextra)

    if(TELEMETRY_WERROR)
        target_compile_options(telemetry PRIVATE -Werror)
    endif()

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
# GoogleMock: never used. To prevent it being built, turn it off and store is a cache value
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

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
    tests/faultSourceTest.cpp
)

# Applying warnings to unit_tests
target_compile_options(unit_tests PRIVATE -Wall -Wextra)

if(TELEMETRY_WERROR)
    target_compile_options(unit_tests PRIVATE -Werror)
endif()

# Linking Testing Executable
target_link_libraries(unit_tests PRIVATE telemetry_core GTest::gtest_main)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)

# Allow tests/ access to fetch from hosts/
target_include_directories(telemetry_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
