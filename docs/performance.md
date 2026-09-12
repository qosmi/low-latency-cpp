# Performance

## Objectives

The performance work focuses on:

- event throughput
- queue overhead
- batch-processing overhead
- reconstruction throughput
- processing latency

## Measurement

The benchmarks use `std::chrono::steady_clock`.

Throughput is calculated as:

    processed_events / elapsed_seconds

The measurements are intended for relative comparison between configurations
on the same machine.

## Batch Processing

The batch benchmark compares multiple batch sizes.

The objective is to determine whether processing events in groups reduces
per-event queue and processing overhead.

Results are hardware- and build-dependent.

## Reconstruction

The reconstruction benchmark measures the throughput of the C++ image
processing path independently from the acquisition simulation.

## Latency

Latency is recorded using the same monotonic clock for the simulated event
timestamp and processing-stage measurement.

Reported statistics include:

- minimum
- average
- maximum
- p50
- p95
- p99

The current measurement does not represent complete hardware-to-image
latency.

## Interpretation

Benchmark numbers should be compared under the same:

- CPU
- compiler
- optimization level
- operating-system conditions
- event workload
- batch size

No universal throughput claim should be inferred from these measurements.