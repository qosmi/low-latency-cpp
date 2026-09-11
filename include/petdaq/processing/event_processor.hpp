#pragma once

#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"

#include <atomic>
#include <cstdint>
#include <thread>

namespace petdaq {

/**
 * Consumes detector events from the acquisition queue.
 *
 * This processor intentionally does very little work in the first version.
 * Later iterations will add calibration, filtering, batching and reconstruction
 * while keeping the acquisition/processing boundary measurable.
 */
template <std::size_t QueueCapacity>
class EventProcessor final {
public:
    using Queue = SpscRingBuffer<DetectorEvent, QueueCapacity>;

    EventProcessor(Queue& queue,
                   PipelineStatistics& statistics,
                   const std::atomic<bool>& producer_done)
        : queue_(queue),
          statistics_(statistics),
          producer_done_(producer_done) {}
    /*
    try_pop()
       │
       ├── success ──► process event
       │                  │
       │                  └── loop
       │
       └── failure
            │
            ├── producer still running ──► yield
            │                                  │
            │                                  └── loop
            │
            └── producer finished ──► terminate
    */
    void run() {
        DetectorEvent event{};

        while (true)
        {
            if (queue_.try_pop(event))
            {
                if (process_one(event))
                {
                    statistics_.processed.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
                continue;
            }

            if (producer_done_.load(std::memory_order_acquire))
            {
                break;
            }

            // The queue is temporarily empty while the producer is still
            // running. Yield rather than spinning continuously.
            std::this_thread::yield();
        }
    }

private:
    [[nodiscard]] bool process_one(const DetectorEvent& event) noexcept {
        // Placeholder for the hot processing path.
        //
        // The volatile sink prevents the compiler from proving that the
        // simulated work has no observable effect. In later versions this
        // will become actual calibration/event processing.
        sink_ ^= static_cast<std::uint64_t>(event.raw_energy)
                  + event.timestamp_ns
                  + event.detector_id
                  + event.channel;
        return true;
    }

    Queue& queue_;
    PipelineStatistics& statistics_;
    const std::atomic<bool>& producer_done_;

    std::uint64_t sink_{0};
};

} // namespace petdaq
