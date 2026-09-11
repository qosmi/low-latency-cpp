#include <atomic>
#include <cassert>
#include <cstdint>
#include <thread>
#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/processing/event_processor.hpp"

int main()
{
    using Event = petdaq::DetectorEvent;
    using Queue = petdaq::SpscRingBuffer<Event, 64>;
    using Processor = petdaq::EventProcessor<64>;

    constexpr std::uint64_t EventCount = 10'000;

    Queue queue;
    petdaq::PipelineStatistics statistics;
    std::atomic<bool> producer_done{false};
    petdaq::LatencyRecorder latency_recorder;
    petdaq::CalibrationTable calibration;

    Processor processor{
        queue,
        statistics,
        producer_done,
        &latency_recorder,
        &calibration,
        32};

    std::thread producer([&]
                         {
        for (std::uint64_t i = 0; i < EventCount; ++i) {
            Event event{
                .timestamp_ns = i,
                .detector_id = 1,
                .channel = 2,
                .raw_energy = static_cast<std::uint32_t>(i)
            };

            while (!queue.try_push(event)) {
                std::this_thread::yield();
            }

            statistics.generated.fetch_add(
                1,
                std::memory_order_relaxed);

            statistics.enqueued.fetch_add(
                1,
                std::memory_order_relaxed);
        }

        producer_done.store(
            true,
            std::memory_order_release); });

    std::thread consumer([&]
                         { processor.run(); });

    producer.join();
    consumer.join();

    assert(
        statistics.generated.load(std::memory_order_relaxed) == EventCount);

    assert(
        statistics.enqueued.load(std::memory_order_relaxed) == EventCount);

    assert(
        statistics.processed.load(std::memory_order_relaxed) == EventCount);

    assert(
        statistics.dropped.load(std::memory_order_relaxed) == 0);

    return 0;
}