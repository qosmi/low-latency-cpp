#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/processing/event_processor.hpp"

template <std::size_t BatchSize>
double run_benchmark(std::uint64_t event_count)
{
    using Event = petdaq::DetectorEvent;
    using Queue = petdaq::SpscRingBuffer<Event, 1 << 16>;
    using Processor = petdaq::EventProcessor<1 << 16>;

    Queue queue;
    petdaq::PipelineStatistics statistics;
    std::atomic<bool> producer_done{false};
    petdaq::LatencyRecorder latency_recorder;
    const petdaq::CalibrationTable calibration;

    Processor processor{
        queue,
        statistics,
        producer_done,
        &latency_recorder,
        &calibration,
        BatchSize};

    const auto start = std::chrono::steady_clock::now();

    std::thread producer([&]
                         {
        for (std::uint64_t i = 0; i < event_count; ++i) {
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

    const auto elapsed = std::chrono::steady_clock::now() - start;

    const double seconds =
        std::chrono::duration<double>(elapsed).count();

    return static_cast<double>(event_count) / seconds;
}

int main()
{
    constexpr std::uint64_t EventCount = 5'000'000;

    std::cout << "Batch processing benchmark\n";
    std::cout << "Events: " << EventCount << "\n\n";

    std::cout << "batch=1  : "
              << run_benchmark<1>(EventCount)
              << " events/s\n";

    std::cout << "batch=8  : "
              << run_benchmark<8>(EventCount)
              << " events/s\n";

    std::cout << "batch=16 : "
              << run_benchmark<16>(EventCount)
              << " events/s\n";

    std::cout << "batch=32 : "
              << run_benchmark<32>(EventCount)
              << " events/s\n";

    std::cout << "batch=64 : "
              << run_benchmark<64>(EventCount)
              << " events/s\n";
}