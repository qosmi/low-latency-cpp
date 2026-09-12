#include <cassert>
#include <atomic>
#include <cstdint>
#include <thread>

#include "petdaq/core/detector_event.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"

int main()
{
    using petdaq::DetectorEvent;
    using petdaq::SpscRingBuffer;

    constexpr std::size_t QueueCapacity = 1024;
    constexpr std::uint64_t EventCount = 1'000'000;
    constexpr int Runs = 20;

    for (int run = 0; run < Runs; ++run) {
        SpscRingBuffer<DetectorEvent, QueueCapacity> queue;

        std::atomic<bool> producer_done{false};
        std::uint64_t consumed = 0;

        std::thread producer([&] {
            for (std::uint64_t i = 0; i < EventCount; ++i) {
                DetectorEvent event{
                    .timestamp_ns = i,
                    .detector_id = 1,
                    .channel =
                        static_cast<std::uint16_t>(i % 64),
                    .raw_energy =
                        static_cast<std::uint32_t>(i)
                };

                while (!queue.try_push(event)) {
                    std::this_thread::yield();
                }
            }

            producer_done.store(
                true,
                std::memory_order_release);
        });

        std::thread consumer([&] {
            DetectorEvent event{};

            while (true) {
                if (queue.try_pop(event)) {
                    assert(event.timestamp_ns == consumed);
                    ++consumed;
                    continue;
                }

                if (producer_done.load(
                        std::memory_order_acquire)) {
                    break;
                }

                std::this_thread::yield();
            }
        });

        producer.join();
        consumer.join();

        assert(consumed == EventCount);
    }

    return 0;
}