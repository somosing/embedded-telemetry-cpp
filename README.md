# Embedded Telemetry C++

A small C++20 project demonstrating byte-stream framing, packet validation,
telemetry decoding, and health-state evaluation for an embedded-style binary
protocol.

## Current pipeline

1. Read a continuous byte stream.
2. Ignore noise until the start marker is detected.
3. Collect one complete eight-byte packet.
4. Validate the header and XOR checksum.
5. Decode little-endian voltage and current values.
6. Convert millivolts and milliamps to engineering units.
7. Classify the telemetry health state.

## Packet format

| Byte | Field |
|---:|---|
| 0 | Start marker `0xAA` |
| 1 | Message ID |
| 2 | Payload length |
| 3 | Voltage low byte |
| 4 | Voltage high byte |
| 5 | Current low byte |
| 6 | Current high byte |
| 7 | XOR checksum |

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/telemetry_demo