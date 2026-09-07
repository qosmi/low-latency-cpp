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

Correctness tests must remain separate from performance tests.
