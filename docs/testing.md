# Testing Strategy

Planned test layers:

## Unit tests

- ring-buffer empty behavior
- ring-buffer full behavior
- FIFO ordering
- wrap-around
- detector-event invariants
- calibration functions

## Integration tests

- producer → queue → consumer
- no duplication
- expected drop behavior under overload
- graceful shutdown

## Performance tests

- throughput
- latency distribution
- queue occupancy
- regression thresholds

## Tooling

The project will add:

- AddressSanitizer
- UndefinedBehaviorSanitizer
- GoogleTest
- Google Benchmark
- Linux `perf`

## Concurrency validation

The SPSC queue is validated at three levels:

1. Functional correctness
   - empty queue behavior
   - FIFO ordering
   - event field preservation
   - full queue behavior

2. Concurrent stress testing
   - one producer
   - one consumer
   - 1,000,000 events per run
   - repeated across multiple runs
   - sequence numbers verify FIFO ordering

3. Sanitizer validation
   - AddressSanitizer
   - UndefinedBehaviorSanitizer

The sanitizer build uses the same test suite as the normal build.

Correctness tests must remain separate from performance tests.
