# Testing

## Unit Tests

Unit tests cover:

- event representation assumptions
- SPSC queue behavior
- calibration
- batching
- latency statistics
- latency percentiles
- pipeline accounting
- reconstruction
- CSV output

## Queue Tests

The SPSC queue tests verify:

- empty queue behavior
- single-event push/pop
- field preservation
- FIFO ordering
- full queue behavior
- bulk dequeue
- partial bulk dequeue

## Concurrency Tests

Producer and consumer threads exchange a large number of sequence-numbered
events.

The consumer validates that sequence numbers are received in order.

The stress test is repeated to increase the likelihood of exposing timing-
dependent concurrency defects.

## Sanitizers

The project supports sanitizer-enabled builds.

Example:

    cmake -S . -B build-asan \
        -DPETDAQ_ENABLE_SANITIZERS=ON \
        -DCMAKE_BUILD_TYPE=Debug

    cmake --build build-asan

    ctest --test-dir build-asan --output-on-failure

## Integration Testing

The reconstruction demonstration exercises:

    DetectorEvent
        →
    EventBatch
        →
    CalibrationTable
        →
    Reconstructor
        →
    CSV writer

The Python tool then consumes the generated reconstruction.

## Performance Testing

Performance tests are separate from correctness tests.

They measure:

- batch-processing throughput
- reconstruction throughput
- latency statistics

Performance results are machine-dependent.