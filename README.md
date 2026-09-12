# PET-DAQ: Low-Latency C++ Event Processing Pipeline

A portfolio project demonstrating high-throughput detector-event processing
using modern C++, a bounded SPSC lock-free queue, batch processing,
calibration, latency measurement, concurrency testing, and a small
educational image-reconstruction pipeline.

> This project is an engineering and educational demonstration. It is not a
> clinically validated PET acquisition or reconstruction system.

## Project Overview

The pipeline simulates detector events entering a data-acquisition path and
being processed by a consumer thread.

The main processing path is:

Detector simulator
    ↓
SPSC ring buffer
    ↓
Bulk dequeue
    ↓
Event batch
    ↓
Energy calibration
    ↓
Image projection
    ↓
Reconstruction CSV
    ↓
Python analysis / visualization

The implementation focuses on deterministic memory usage, concurrency
correctness, throughput, and measurable latency.

## Engineering Goals

The project demonstrates:

- Modern C++17
- Bounded lock-free SPSC communication
- Acquire/release memory ordering
- Cache-line separation of producer/consumer indices
- Batch event processing
- Bulk queue dequeue
- Channel-specific calibration
- Host monotonic-clock latency measurement
- p50/p95/p99 latency reporting
- Pipeline accounting invariants
- Concurrent stress testing
- AddressSanitizer validation
- Deterministic image reconstruction
- C++ to Python data analysis
- Reproducible synthetic data generation
- Performance benchmarking

## Architecture

### Acquisition

The detector simulator produces fixed-size `DetectorEvent` objects.

Events contain:

- timestamp
- detector ID
- channel
- raw energy

The simulator writes events into a bounded SPSC queue.

### SPSC Queue

The queue uses a power-of-two capacity and a bit mask for index-to-storage
mapping.

The producer owns the write index.

The consumer owns the read index.

The implementation uses:

- relaxed operations for each thread's own index
- acquire operations when observing the other thread's progress
- release operations when publishing progress

No mutex is used.

The queue uses fixed storage and does not dynamically allocate memory while
processing events.

### Batch Processing

The consumer dequeues multiple events using `try_pop_bulk()`.

The bulk operation reduces the number of queue-index synchronization
operations required per event.

Events are processed through an `EventBatch`.

### Calibration

Raw detector energy is transformed using channel-specific pedestal and gain
constants:

    calibrated_energy = (raw_energy - pedestal) * gain

The raw event representation remains separate from the calibrated
representation.

### Latency

Events are timestamped using `std::chrono::steady_clock`.

Latency is measured when the event reaches the processing stage.

The recorder reports:

- sample count
- minimum
- average
- maximum
- p50
- p95
- p99

The measurement includes queue waiting time but is not a complete
hardware-to-image clinical latency measurement.

### Reconstruction

The educational reconstruction maps calibrated detector events into a
two-dimensional floating-point image.

The project also contains a deterministic synthetic phantom generator so the
output can be visualized and inspected.

The reconstruction algorithm is intentionally simple and is not a clinically
validated PET reconstruction algorithm.

## Testing

The project contains tests for:

- SPSC queue behavior
- FIFO ordering
- full queue behavior
- bulk dequeue
- concurrent producer/consumer operation
- calibration
- latency statistics
- latency percentiles
- pipeline accounting
- event batching
- reconstruction
- image output

Concurrency stress testing is also performed with repeated high-volume
producer/consumer workloads.

Sanitizer builds use AddressSanitizer and related compiler instrumentation
when enabled.

## Building

Configure:

    cmake -S . -B build

Build:

    cmake --build build

Run tests:

    ctest --test-dir build --output-on-failure

## Sanitizer Build

Configure:

    cmake -S . -B build-asan \
        -DPETDAQ_ENABLE_SANITIZERS=ON \
        -DCMAKE_BUILD_TYPE=Debug

Build:

    cmake --build build-asan

Run tests:

    ctest --test-dir build-asan --output-on-failure

## Reconstruction Demo

Run:

    ./build/pet_daq_reconstruction_demo

This generates:

    reconstruction.csv

Analyze it using:

    python3 tools/python/analyze_results.py

The Python tool reports basic image statistics and generates a visualization.

## Benchmarks

Batch processing benchmark:

    ./build/pet_daq_batch_processing

Reconstruction benchmark:

    ./build/pet_daq_reconstruction_processing

Benchmark results are machine-dependent. Measured values should be recorded
with the CPU, compiler, build configuration, and operating environment.

### Batch Processing

| Batch size | Processed events | Time (ms) | Throughput (events/s) |
|------------|------------------|-----------|------------------------|
| 1          | 1.75446e+07      |           |                        |
| 4          | 1.7425e+07       |           |                        |
| 16         | 1.68834e+07      |           |                        |
| 32         | 1.6602e+07       |           |                        |
| 64         | 1.72931e+07      |           |                        |

### Reconstruction

| Events  | Projected | Rejected | Time (ms) | Throughput (events/s) |
|---------|-----------|----------|-----------|-----------------------|
| 5000000 |  5000000  |    0     |  180.79   |        27656940       |

## Project Structure

    include/
        petdaq/
            core/
            daq/
            processing/
            reconstruction/

    src/
        main.cpp

    tests/
        queue and pipeline tests

    benchmarks/
        performance benchmarks

    examples/
        reconstruction demonstration

    tools/
        python/
            analysis and visualization

    docs/
        requirements
        architecture
        testing
        risk analysis
        performance
        engineering notes

## Engineering Trade-offs

### Why SPSC?

The simulated acquisition path has one producer and one consumer, allowing
the synchronization problem to be simplified substantially compared with a
multi-producer/multi-consumer queue.

### Why bounded storage?

The processing path should avoid unpredictable allocation behavior during
event processing.

### Why batching?

Batching amortizes queue synchronization and provides a natural boundary
for later vectorized or parallel processing.

### Why `steady_clock`?

Latency measurement requires a monotonic clock. Wall-clock adjustments should
not affect elapsed-time measurements.

### Why fixed-size event structures?

A compact trivially-copyable event representation is suitable for high-rate
transport through the queue and makes ownership and memory behavior easier
to reason about.

## Limitations

This project does not claim:

- clinical validity
- medical-device certification
- real detector electronics integration
- clinically accurate PET reconstruction
- real hardware timing accuracy
- production regulatory compliance

The architecture is intended to demonstrate software-engineering principles
relevant to high-throughput scientific and medical-device data processing.

## License

This project is intended as an educational and portfolio piece and is licensed under the MIT License, meaning you're free to use, modify, and distribute the code — see the [LICENSE](LICENSE) file for the full text.