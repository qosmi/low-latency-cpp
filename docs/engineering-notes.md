# Engineering Notes

## 1. SPSC queue

The acquisition simulation and processing stage communicate through a
bounded single-producer/single-consumer ring buffer.

The queue uses atomic producer and consumer indices instead of a mutex.

The producer owns the write index and the consumer owns the read index.
Each side uses relaxed memory ordering when accessing its own index and
acquire/release synchronization when observing the other side.

The queue capacity is a power of two so index-to-storage mapping can use a
bit mask instead of modulo arithmetic.

## 2. Bounded memory

The queue uses fixed storage and does not dynamically allocate while the
pipeline is running.

Event batches also use fixed-size storage.

This makes memory behavior predictable and avoids allocator activity in
the processing path.

## 3. Batch processing

Events are dequeued in batches rather than performing one queue
synchronization operation for every event.

The purpose is to amortize queue synchronization and improve throughput.

The batch size is configurable at the processing boundary and is measured
using a dedicated benchmark.

## 4. Latency measurement

The simulator timestamps events using the host monotonic clock.

Processing latency is measured when an event reaches the processing stage.

The measured interval therefore represents acquisition timestamp to
processing-stage measurement and includes queue waiting time.

This is not intended to represent a complete hardware-to-image clinical
latency measurement.

## 5. Calibration

Raw detector energy is transformed using channel-specific pedestal and
gain constants.

Calibration is kept separate from event acquisition so the acquisition
representation remains raw.

## 6. Reconstruction

The demonstration reconstruction maps calibrated events into a two-
dimensional image.

The implementation is deliberately educational and is not a clinically
validated PET reconstruction algorithm.

## 7. Testing

The project includes:

- unit tests
- SPSC FIFO tests
- concurrent stress testing
- sanitizer builds
- pipeline accounting checks
- reconstruction tests
- CSV output tests
- throughput benchmarks

## 8. Performance methodology

Performance measurements are made with dedicated benchmark executables.

Reported values are machine-dependent and should not be interpreted as
universal performance characteristics.