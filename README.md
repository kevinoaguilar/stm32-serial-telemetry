# STM32 Serial Telemetry

An STM32 NUCLEO-F411RE board streams framed telemetry packets over UART, and a Windows C++17 host program reads the serial port, reassembles the byte stream into packets, validates them, and parses them.

Both halves are written from scratch. The firmware is in C on the STM32 HAL, and the host is in C++17 using the Win32 serial API, with no third-party libraries.

## How it works

```
┌──────────────────────┐   UART 115200 8N1   ┌──────────┐   USB    ┌───────────────────────────────┐
│ STM32F411 (firmware) │ ──────────────────► │ ST-Link  │ ───────► │ Windows host (C++17)          │
│ builds packet, sends │      USART2 TX      │ VCP      │  COM5    │ SerialPort → PacketReader →   │
│ once per second      │                     └──────────┘          │ Parser → printed results      │
└──────────────────────┘                                           └───────────────────────────────┘
```

### Packet format

| byte | field | meaning |
|---|---|---|
| 0 | header | always `0xAA` |
| 1 | length | number of payload bytes that follow |
| 2 … length+1 | payload | the data |
| length+2 | checksum | sum of the payload bytes, mod 256 |

The firmware currently sends a 3-byte payload, `11 22 <counter>`. The counter goes up by one with every packet, so a gap in the counter shows that packets were lost.

### Host design

| component | role |
|---|---|
| `DataSource` | interface: `readChunk()` returns whatever bytes are available (it may be empty), and `isFinished()` reports end of stream |
| `SerialPort` | `DataSource` over a Windows COM port (RAII handle, 8N1, 1 s read timeout); throws on I/O errors |
| `ChunkListSource` | `DataSource` that replays hand-written chunks, used for testing without hardware |
| `PacketReader` | reassembles a byte stream into packets. It keeps leftover bytes between reads, resyncs on `0xAA`, and handles packets split across reads |
| `Parser` | validates a single packet (header, length against the bytes actually received, checksum) |
| `Pipeline` | `runPipeline(source, reader, maxReads)` is one loop that works with any `DataSource` |
| `PacketStats` | running valid/invalid counts and the valid rate |

Because `runPipeline` only depends on the `DataSource` interface, the same code runs against the real board and against scripted test data.

## Repository layout

```
firmware/Telemetry_Firmware_F411/   STM32CubeIDE project (send loop in Core/Src/main.c)
host/                               C++17 host: library code + test programs
```

## Building and running

### Firmware
1. Import `firmware/Telemetry_Firmware_F411` into STM32CubeIDE (File → Import → Existing Projects into Workspace).
2. Build and run it on a NUCLEO-F411RE. The green LED blinks once per packet sent.

### Host (Windows, GCC via MSYS2)
From `host/`:
```
g++ -Wall -Wextra -std=c++17 serialTest.cpp SerialPort.cpp Pipeline.cpp PacketReader.cpp Parser.cpp -o serialTest
.\serialTest
```
Test programs that don't need the board:
```
g++ -Wall -Wextra -std=c++17 pipelineTest.cpp ChunkListSource.cpp Pipeline.cpp PacketReader.cpp Parser.cpp -o pipelineTest
g++ -Wall -Wextra -std=c++17 readerTest.cpp PacketReader.cpp Parser.cpp -o readerTest
```
`serialTest` opens `COM5`. Check which COM port the board uses in Device Manager → Ports (COM & LPT).

### Example output
A real `serialTest` run with the board connected. Packets often arrive split across reads, for example `aa` in one read and the remaining 5 bytes in the next, and `PacketReader` reassembles them:
```
opened
Chunks received: 1
Packets created: 0
Chunks received: 2
Packets created: 1
 VALID    ->    Payload size : 3
11 22 8
Chunks received: 3
Packets created: 0
Chunks received: 4
Packets created: 1
 VALID    ->    Payload size : 3
11 22 9
Chunks received: 5
Packets created: 0
Chunks received: 6
Packets created: 0
Chunks received: 7
Packets created: 1
 VALID    ->    Payload size : 3
11 22 a
COM5 port is now closed.
end of main
```

## Known limitations

These are known, and the planned framing upgrade below addresses them:
- **Length-field stall:** a stray `0xAA` followed by a large length byte makes the reader wait for bytes that will never form that packet.
- **Weak checksum:** a one-byte sum catches far fewer errors than a CRC.
- **No delivery guarantee:** bytes sent while no program has the port open may be dropped. The counter makes these gaps visible.

## Roadmap

- [x] Firmware UART send loop; host receives, reassembles and validates packets
- [x] `DataSource` interface; `SerialPort` and test sources share one pipeline
- [ ] `std::unique_ptr` ownership of sources; file-replay source and logging
- [ ] COBS framing with a `0x00` delimiter, plus CRC-16, on both ends
- [ ] Fault-injecting source for robustness tests
- [ ] CMake build, GoogleTest unit tests, CI
