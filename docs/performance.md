# Performance

This document will contain measured results rather than theoretical claims.

## Current benchmark

The first implementation reports:

- generated events
- enqueued events
- processed events
- dropped events
- wall-clock throughput

These numbers are machine-dependent.

## Planned measurements

The next iteration will measure:

- per-event latency
- p50
- p95
- p99
- p99.9
- maximum observed latency
- queue occupancy
- CPU utilization
- context switches
- cache misses
- branch misses

## Optimization policy

Every optimization should answer three questions:

1. What was measured?
2. What changed?
3. What improved, and at what cost?

No optimization result should be reported without a reproducible benchmark.
