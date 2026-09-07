#pragma once

#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <random>

namespace petdaq {

/**
 * Generates synthetic detector events.
 *
 * The simulator represents the hardware-facing side of the system. A real
 * implementation would replace this component with a DMA/driver interface.
 */
template <std::size_t QueueCapacity>
class DetectorSimulator final {
public:
    using Queue = SpscRingBuffer<DetectorEvent, QueueCapacity>;

    DetectorSimulator(Queue& queue,
                      PipelineStatistics& statistics,
                      std::uint64_t event_count)
        : queue_(queue),
          statistics_(statistics),
          event_count_(event_count) {}

    void run(std::atomic<bool>& producer_done) {
        std::mt19937 rng{42}; // Fixed seed makes demo runs reproducible.
        std::uniform_int_distribution<std::uint16_t> detector_dist{0, 255};
        std::uniform_int_distribution<std::uint16_t> channel_dist{0, 63};
        std::uniform_int_distribution<std::uint32_t> energy_dist{0, 4095};

        for (std::uint64_t i = 0; i < event_count_; ++i) {
            DetectorEvent event{
                .timestamp_ns = i * 50, // Simulated 20 MHz event clock.
                .detector_id = detector_dist(rng),
                .channel = channel_dist(rng),
                .raw_energy = energy_dist(rng)
            };

            statistics_.generated.fetch_add(1, std::memory_order_relaxed);

            // A non-blocking DAQ path must decide what to do when the bounded
            // buffer is full. This first version records the loss explicitly.
            if (queue_.try_push(event)) {
                statistics_.enqueued.fetch_add(1, std::memory_order_relaxed);
            } else {
                statistics_.dropped.fetch_add(1, std::memory_order_relaxed);
            }
        }

        producer_done.store(true, std::memory_order_release);
    }

private:
    Queue& queue_;
    PipelineStatistics& statistics_;
    std::uint64_t event_count_;
};

} // namespace petdaq
