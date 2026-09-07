# PET-DAQ

A portfolio project demonstrating a low-latency C++ data-acquisition and processing pipeline for a simulated PET detector system.

> **Portfolio / educational project:** this repository is a simulation and is not medical-device software and is not clinically validated.

## Current status

**Phase 1:** working detector simulator → bounded SPSC ring buffer → processing consumer.

The initial implementation intentionally favors clarity. Later phases will measure and optimize it using benchmarks and profiling.

## Architecture

```text
+-------------------+
| Detector Simulator|
|  (producer)       |
+---------+---------+
          |
          | DetectorEvent
          v
+-------------------+
| Bounded SPSC      |
| Ring Buffer       |
+---------+---------+
          |
          v
+-------------------+
| Event Processor   |
|  (consumer)       |
+---------+---------+
          |
          v
+-------------------+
| Statistics /      |
| Monitoring        |
+-------------------+
```

## Build

Requirements:

- C++20 compiler
- CMake 3.20+
- Linux/macOS/Windows

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run:

```bash
./build/pet_daq_demo
```

Example:

```text
PET-DAQ low-latency acquisition demo
Generating 5,000,000 detector events...
Processed: 4,999,xxx
Dropped:    xxx
Elapsed:    ... ms
Throughput: ... events/s
```

The exact numbers depend on the machine and OS.

## Project roadmap

- [x] Detector-event model
- [x] Simulated detector producer
- [x] Bounded SPSC ring buffer
- [x] Consumer processing loop
- [x] Basic throughput/drop statistics
- [ ] Latency histogram and p99/p99.9 measurement
- [ ] Calibration pipeline
- [ ] Batch processing
- [ ] Coincidence/event selection
- [ ] Sinogram generation
- [ ] Simplified image reconstruction
- [ ] Google Benchmark suite
- [ ] Unit/integration tests
- [ ] Python analysis tools
- [ ] `perf` profiling report
- [ ] Cache/SIMD optimization
- [ ] Requirements/risk documentation

## Engineering goals

The project is designed around four properties relevant to high-rate acquisition:

1. **Bounded memory:** the hot path does not dynamically grow the queue.
2. **Single-producer/single-consumer ownership:** this permits a simpler synchronization strategy.
3. **Measurement before optimization:** performance changes will be supported by benchmarks.
4. **Deterministic behavior where practical:** overload is explicit and observable through dropped-event counters.

## Disclaimer

This project is for software-engineering education and portfolio demonstration. It does not implement a clinically validated PET system and must not be used for diagnosis, treatment, or patient care.
