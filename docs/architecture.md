# Architecture

## Phase 1

The current system contains two execution contexts:

### Producer

`DetectorSimulator`

- creates detector events
- timestamps them
- attempts a non-blocking enqueue
- records dropped events

### Consumer

`EventProcessor`

- removes events from the queue
- performs a minimal processing operation
- records processed events

## Synchronization

The boundary is an SPSC bounded ring buffer.

The design assumes:

- exactly one producer
- exactly one consumer
- fixed capacity
- no dynamic allocation in the queue

This is intentionally a constrained design. If the real system requires multiple producers or consumers, the architecture must be reconsidered rather than simply adding more atomics.

## Next phase

The processing path will evolve toward:

```text
DAQ
 ↓
event validation
 ↓
calibration
 ↓
energy/time filtering
 ↓
coincidence detection
 ↓
sinogram
 ↓
reconstruction
```
