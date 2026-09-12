# Architecture

## 1. System Context

The project models a simplified detector acquisition and processing system.

The acquisition simulator produces detector events and submits them to a
bounded single-producer/single-consumer queue.

A processing thread consumes those events, applies calibration, and performs
image reconstruction in the reconstruction demonstration.

## 2. Data Flow

    DetectorSimulator
          |
          | DetectorEvent
          v
    +-------------------+
    | SPSC Ring Buffer  |
    +-------------------+
          |
          | bulk dequeue
          v
    +-------------------+
    | EventBatch        |
    +-------------------+
          |
          v
    +-------------------+
    | CalibrationTable  |
    +-------------------+
          |
          | CalibratedEvent
          v
    +-------------------+
    | EventProjector    |
    +-------------------+
          |
          v
    +-------------------+
    | Image2D           |
    +-------------------+
          |
          v
    reconstruction.csv
          |
          v
    Python analysis
          |
          v
    reconstruction.png

## 3. Event Representation

`DetectorEvent` represents the transport-level event.

It contains:

- timestamp
- detector ID
- channel
- raw energy

The event is intentionally compact and trivially copyable.

`CalibratedEvent` represents the processing-level event after applying
channel-specific calibration.

Keeping the two representations separate makes the transformation explicit.

## 4. Queue Design

`SpscRingBuffer` assumes exactly one producer and one consumer.

The producer owns:

    write_index_

The consumer owns:

    read_index_

The producer:

1. reads its own write index
2. acquires the consumer read index
3. checks for available capacity
4. writes the event
5. releases the new write index

The consumer:

1. reads its own read index
2. acquires the producer write index
3. checks whether data exists
4. reads the event
5. releases the new read index

## 5. Bulk Dequeue

The queue also provides:

    try_pop_bulk()

Instead of performing an atomic index publication for every event, the
consumer can retrieve several available events and publish its new read
position once.

This is intended to reduce synchronization overhead.

The queue still maintains the same producer/consumer ownership model.

## 6. Memory Ordering

The implementation deliberately uses acquire/release synchronization.

The producer's release operation publishes event data before advancing the
write index.

The consumer's acquire operation observes that publication before reading
the corresponding event.

Likewise, consumer progress is published with a release operation so the
producer can safely determine which storage slots can be reused.

## 7. Memory Allocation

The queue uses:

    std::array<T, Capacity>

and therefore has fixed storage.

The event batch also has fixed storage.

The core event-processing path therefore does not depend on dynamic memory
allocation for individual events.

## 8. Processing

`EventProcessor` drains the queue into an `EventBatch`.

The batch is then processed as a unit.

The processing stage can optionally:

- record latency
- calibrate events
- perform downstream processing

## 9. Reconstruction

The reconstruction demonstration converts calibrated events into a
two-dimensional image.

The projector maps detector/channel coordinates to image coordinates.

This is intentionally a simple educational reconstruction model.

It should not be interpreted as an implementation of a clinical PET
reconstruction algorithm.

## 10. Python Interface

The C++ reconstruction writes a CSV representation of the image.

Python loads this data using NumPy and computes summary statistics.

Matplotlib is used to create a visual representation.

This separation demonstrates a common engineering workflow where
performance-critical processing remains in C++ while exploratory analysis
and visualization are performed in Python.