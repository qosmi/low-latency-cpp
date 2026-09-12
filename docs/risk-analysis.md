# Risk Analysis — Educational Simulation

This document demonstrates the style of reasoning used in safety-oriented engineering. It is **not** a medical-device risk analysis.

| Hazard | Possible cause | Control in this project |
|---|---|---|
| Event loss | Queue full | Count and report dropped events |
| Event corruption | Concurrency bug | SPSC ownership model + tests |
| Silent performance regression | Code change | Performance benchmark |
| Invalid processing result | Bad calibration | Unit tests + test fixtures |

A real medical-device project would require a formal risk-management process and applicable regulatory documentation.

# Risk Analysis

| Risk | Cause | Effect | Mitigation |
|------|-------|--------|------------|
| Event loss | Queue full | Missing events | Drop counter and accounting invariant |
| Event duplication | Incorrect queue indexing | Incorrect reconstruction | FIFO and sequence-number stress tests |
| Event reordering | Incorrect synchronization | Incorrect processing order | Concurrent sequence validation |
| Data race | Incorrect memory ordering | Undefined behavior | Acquire/release design and sanitizer testing |
| Queue overflow | Producer exceeds consumer | Dropped events | Bounded queue with explicit drop accounting |
| Stale data | Slot reused incorrectly | Corrupted events | Producer/consumer ownership and release/acquire synchronization |
| Invalid channel | Out-of-range calibration lookup | Invalid memory access | Explicit channel validation |
| Reconstruction rejection | Coordinates outside image | Missing projected event | Rejection statistics |
| Timing error | Different clock domains | Invalid latency measurement | Monotonic clock and documented scope |

## Scope

This risk analysis addresses software-engineering risks in the educational
pipeline.

It does not constitute a medical-device risk-management file and does not
claim compliance with EN ISO 14971.
