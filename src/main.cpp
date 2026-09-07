#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/detector_simulator.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/processing/event_processor.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>

namespace {
constexpr std::size_t QueueCapacity = 1U << 16U;
constexpr std::uint64_t EventCount = 5'000'000;

void print_statistics(const petdaq::PipelineStatistics& stats,
                      std::chrono::steady_clock::duration elapsed) {
    const auto generated = stats.generated.load(std::memory_order_relaxed);
    const auto enqueued = stats.enqueued.load(std::memory_order_relaxed);
    const auto dropped = stats.dropped.load(std::memory_order_relaxed);
    const auto processed = stats.processed.load(std::memory_order_relaxed);

    const double seconds =
        std::chrono::duration<double>(elapsed).count();

    const double throughput =
        seconds > 0.0 ? static_cast<double>(processed) / seconds : 0.0;

    std::cout << "\nResults\n"
              << "-------\n"
              << "Generated:  " << generated << '\n'
              << "Enqueued:   " << enqueued << '\n'
              << "Processed:  " << processed << '\n'
              << "Dropped:    " << dropped << '\n'
              << std::fixed << std::setprecision(2)
              << "Elapsed:    " << seconds * 1000.0 << " ms\n"
              << "Throughput: " << throughput << " events/s\n";
}

} // namespace

int main() {
    using Queue = petdaq::SpscRingBuffer<
        petdaq::DetectorEvent,
        QueueCapacity>;

    std::cout << "PET-DAQ low-latency acquisition demo\n"
              << "------------------------------------\n"
              << "Events:  " << EventCount << '\n'
              << "Queue:   " << QueueCapacity << " events\n";

    Queue queue;
    petdaq::PipelineStatistics statistics;
    std::atomic<bool> producer_done{false};

    petdaq::DetectorSimulator<QueueCapacity> simulator{
        queue,
        statistics,
        EventCount
    };

    petdaq::EventProcessor<QueueCapacity> processor{
        queue,
        statistics,
        producer_done
    };

    const auto start = std::chrono::steady_clock::now();

    // The producer represents the acquisition side and the consumer represents
    // the processing side. They run concurrently so the queue is exercised as
    // it would be in a streaming system.
    std::thread producer([&] {
        simulator.run(producer_done);
    });

    std::thread consumer([&] {
        processor.run();
    });

    producer.join();
    consumer.join();

    const auto elapsed = std::chrono::steady_clock::now() - start;

    print_statistics(statistics, elapsed);

    return 0;
}
