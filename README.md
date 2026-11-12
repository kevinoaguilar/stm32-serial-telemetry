# STM32 Serial Telemetry

An STM32 NUCLEO-F411RE board streams framed telemetry packets over UART, and a Windows C++17 host program reads them, splits the byte stream into frames, verifies each one with a CRC, and prints the result.

Both halves are written from scratch. The firmware is in C on the STM32 HAL, and the host is in C++17 using the Win32 serial API, with no third-party libraries.

## How it works

```mermaid
flowchart LR
    A["STM32F411 firmware<br/>payload + CRC-16 → COBS → 0x00"] -->|"UART 115200 8N1 (USART2)"| B["ST-Link<br/>virtual COM port"]
    B -->|"USB (COM5)"| C["DataSource<br/>(SerialPort, FileSource, ...)"]
    subgraph Host["Windows host (C++17)"]
        subgraph RT["reader thread"]
            C
        end
        C -->|"push"| Q["ChunkQueue<br/>mutex + condition_variable"]
        subgraph MT["main thread"]
            D["PacketReader<br/>split at 0x00"] --> E["parseFrame<br/>COBS decode + CRC check"] --> F["printed results"]
        end
        Q -->|"pop"| D
    end
```

### Frame format

```
COBS( payload | crc_hi | crc_lo ) | 0x00
```

| part | meaning |
|---|---|
| payload | the data; the firmware currently sends 3 bytes, `11 22 <counter>` |
| `crc_hi crc_lo` | CRC-16/CCITT of the payload (init `0xFFFF`, polynomial `0x1021`), high byte first |
| COBS | Consistent Overhead Byte Stuffing: rewrites the bytes so `0x00` never appears inside a frame (adds exactly 1 byte) |
| `0x00` | delimiter: marks the end of every frame |

Because `0x00` can only mean "end of frame", the reader never has to trust a header or a length byte. A corrupted frame costs exactly that one frame, and the next `0x00` resynchronises the stream. The counter goes up by one with every frame, so a gap in it shows that frames were lost.

Example on the wire, counter `0x05`: `06 11 22 05 88 EE 00`.

### Host design

| component | role |
|---|---|
| `DataSource` | interface: `readChunk()` returns whatever bytes are available (it may be empty), and `isFinished()` reports end of stream |
| `SerialPort` | `DataSource` over a Windows COM port (RAII handle, 8N1, 1 s read timeout); throws on I/O errors |
| `ChunkListSource` | `DataSource` that replays hand-written chunks, used for testing without hardware |
| `RecordingSource` | wraps another `DataSource` (owned through `std::unique_ptr`) and writes every byte it passes on to a file |
| `FileSource` | `DataSource` that replays a recorded file in fixed-size chunks |
| `PacketReader` | keeps leftover bytes between reads and cuts a frame at each `0x00`, so frames split across reads are reassembled |
| `parseFrame` | COBS-decodes one frame and checks its CRC-16 |
| `Cobs`, `Crc16` | `cobsEncode` / `cobsDecode` and `crc16`, written with pointer + length so the same code runs on the board |
| `ChunkQueue` | thread-safe FIFO of chunks: `push`, a blocking `pop` (sleeps on a `std::condition_variable`), and `close()` to signal end of stream; chunks are moved in and out, never copied |
| `ReaderThread` | owns a `std::thread` that loops on `readChunk()` and pushes each non-empty chunk; stops on a `std::atomic<bool>` flag or at end of stream, catches exceptions inside the thread and reports them, always calls `close()`, and joins in its destructor (RAII) |
| `Pipeline` | `runPipeline(source, reader, maxReads)` starts a `ReaderThread`, then pops and parses chunks on the main thread; works with any `DataSource` |

`telemetry` picks its source at runtime and owns it through `std::unique_ptr<DataSource>`. Because `runPipeline` only depends on the `DataSource` interface, the same code runs against the real board, a recorded file, or scripted test data.

### Threading

Reading and parsing run on two threads. The reader thread is the only code that touches the `DataSource`, and the main thread is the only code that touches `PacketReader`, so the `ChunkQueue` is the one shared object and the only place that needs a lock. The lock is held only while a chunk is moved in or out, and the main thread sleeps while the queue is empty instead of polling.

Shutdown is designed so the program cannot hang or crash on exit:
- Every way the reader can stop (end of stream, stop requested, or an exception such as the board being unplugged) ends with `close()`, which wakes the main thread.
- An exception in the reader is caught on the reader thread, saved, and printed by the main thread after `join()` as `Reader stopped: <message>`. Unplugging the board mid-run ends the program cleanly and still closes the COM port.
- `ReaderThread` joins in its destructor, so leaving `runPipeline` early, even through an exception, never destroys a joinable thread.

## Repository layout

```
firmware/Telemetry_Firmware_F411/   STM32CubeIDE project (send loop in Core/Src/main.c, crc16.c, cobs.c)
host/                               C++17 host: telemetry program, sources, reader, parser, tests
host/samples/capture.bin            a real recording from the board, for replay without hardware
```

## Building and running

### Firmware
1. Import `firmware/Telemetry_Firmware_F411` into STM32CubeIDE (File → Import → Existing Projects into Workspace).
2. Build and run it on a NUCLEO-F411RE. The green LED blinks once per frame sent.

### Host (Windows, GCC via MSYS2)
From `host/`:
```
g++ -Wall -Wextra -std=c++17 -pthread telemetry.cpp Pipeline.cpp ReaderThread.cpp ChunkQueue.cpp SerialPort.cpp ChunkListSource.cpp RecordingSource.cpp FileSource.cpp PacketReader.cpp Parser.cpp Cobs.cpp Crc16.cpp -o telemetry
```

| command | what it does |
|---|---|
| `.\telemetry serial` | read live frames from the board on `COM5` |
| `.\telemetry record <file>` | read live frames and save every raw byte to `<file>` |
| `.\telemetry replay <file>` | replay a recording, no board needed (try `samples\capture.bin`) |
| `.\telemetry test` | run built-in test chunks: split frames, a zero byte inside a payload, and a corrupted CRC |

Unit tests for the framing code:
```
g++ -Wall -Wextra -std=c++17 crcTest.cpp Crc16.cpp -o crcTest
g++ -Wall -Wextra -std=c++17 cobsTest.cpp Cobs.cpp -o cobsTest
```
`crcTest` checks the standard CRC-16/CCITT check value (`0x29B1` for `"123456789"`). `cobsTest` checks that encode followed by decode returns the original bytes, and that a malformed frame is rejected.

`telemetry serial` opens `COM5`. Check which COM port the board uses in Device Manager → Ports (COM & LPT).

### Example output
A real `telemetry serial` run with the board connected. Frames often arrive split across reads, and `PacketReader` reassembles them:
```
Chunks received: 1
Packets created: 0
Chunks received: 2
Packets created: 0
Chunks received: 3
Packets created: 1
 VALID    ->    Payload size : 3
11 22 10
Chunks received: 4
Packets created: 0
Chunks received: 5
Packets created: 1
 VALID    ->    Payload size : 3
11 22 11
...
COM5 port is now closed.
```

## Known limitations

- **Frame size:** the COBS implementation handles frames shorter than 254 bytes, which covers the current 5-byte frames. Longer frames need the extra `0xFF` code-block rule.
- **No delivery guarantee:** bytes sent while no program has the port open may be dropped, and some of them are stored and arrive all at once in the first read. The counter makes these gaps visible, and the first frame of that backlog can be cut off and reported as `INVALID`. Clearing the input buffer with `PurgeComm` after opening the port would avoid this.
- **Read limit:** `runPipeline` stops after `maxReads` non-empty chunks, so a long recording is only partly replayed. If the board is connected but sends nothing, no chunks arrive and the program waits until Ctrl+C.
- **Shutdown time:** the reader thread checks its stop flag between reads, so stopping can take up to the 1 s serial read timeout.
- **Unbounded queue:** if parsing ever fell behind reading, the queue would keep growing. A bounded queue would make the reader wait instead.

## Roadmap

- [x] Firmware UART send loop; host receives, reassembles and validates packets
- [x] `DataSource` interface; serial, test, recording and file-replay sources share one pipeline
- [x] `std::unique_ptr` ownership of sources, chosen at runtime
- [x] COBS framing with a `0x00` delimiter, plus CRC-16, on both ends
- [x] Background reader thread with a thread-safe queue (`std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`) and clean shutdown
- [ ] Fault-injecting source for robustness tests
- [ ] CMake build, GoogleTest unit tests, CI
