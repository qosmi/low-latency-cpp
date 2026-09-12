#include <cassert>
#include <cstdint>
#include <atomic>
#include <thread>
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/core/detector_event.hpp"

int main()
{
    using petdaq::DetectorEvent;
    using petdaq::SpscRingBuffer;

    SpscRingBuffer<DetectorEvent, 4> queue;

    // ------------------------------------------------------------
    // Test 1: Newly constructed queue is empty.
    // ------------------------------------------------------------
    {
        DetectorEvent event{};
        assert(!queue.try_pop(event));
    }

    // ------------------------------------------------------------
    // Test 2: Push and pop preserve the complete event.
    // ------------------------------------------------------------
    {
        const DetectorEvent input{
            .timestamp_ns = 12345,
            .detector_id = 7,
            .channel = 3,
            .raw_energy = 987};

        assert(queue.try_push(input));

        DetectorEvent output{};

        assert(queue.try_pop(output));

        assert(output.timestamp_ns == input.timestamp_ns);
        assert(output.detector_id == input.detector_id);
        assert(output.channel == input.channel);
        assert(output.raw_energy == input.raw_energy);

        assert(!queue.try_pop(output));
    }

    // ------------------------------------------------------------
    // Test 3: FIFO ordering.
    // ------------------------------------------------------------
    for (std::uint64_t i = 0; i < 4; ++i)
    {
        DetectorEvent e{
            .timestamp_ns = i,
            .detector_id = 1,
            .channel = static_cast<std::uint16_t>(i),
            .raw_energy = static_cast<std::uint32_t>(100 + i)};

        assert(queue.try_push(e));
    }

    for (std::uint64_t i = 0; i < 4; ++i)
    {
        DetectorEvent e{};

        assert(queue.try_pop(e));

        assert(e.timestamp_ns == i);
        assert(e.channel == i);
        assert(e.raw_energy == 100 + i);
    }

    // ------------------------------------------------------------
    // Test 4: Queue rejects pushes when full.
    // ------------------------------------------------------------
    for (std::uint64_t i = 0; i < 4; ++i)
    {
        DetectorEvent e{
            .timestamp_ns = i,
            .detector_id = 2,
            .channel = 0,
            .raw_energy = 200};

        assert(queue.try_push(e));
    }

    DetectorEvent extra{
        .timestamp_ns = 999,
        .detector_id = 9,
        .channel = 9,
        .raw_energy = 999};

    assert(!queue.try_push(extra));

    // ------------------------------------------------------------
    // Test 5: Full queue still contains the original events.
    // ------------------------------------------------------------
    for (std::uint64_t i = 0; i < 4; ++i)
    {
        DetectorEvent e{};

        assert(queue.try_pop(e));
        assert(e.timestamp_ns == i);
    }

    SpscRingBuffer<DetectorEvent, 8> bulk_queue;

    for (std::uint64_t i = 0; i < 6; ++i) {
        DetectorEvent event{
            .timestamp_ns = i,
            .detector_id = 1,
            .channel =
                static_cast<std::uint16_t>(i),
            .raw_energy =
                static_cast<std::uint32_t>(100 + i)
        };

        assert(bulk_queue.try_push(event));
    }

    {
        DetectorEvent output[4]{};

        const std::size_t first_count =
            bulk_queue.try_pop_bulk(output, 4);

        assert(first_count == 4);

        for (std::uint64_t i = 0; i < 4; ++i) {
            assert(output[i].timestamp_ns == i);
            assert(output[i].channel == i);
            assert(output[i].raw_energy == 100 + i);
        }

        const std::size_t second_count =
            bulk_queue.try_pop_bulk(output, 4);

        assert(second_count == 2);

        assert(output[0].timestamp_ns == 4);
        assert(output[1].timestamp_ns == 5);

        assert(
            bulk_queue.try_pop_bulk(output, 4) == 0);
    }

    // ------------------------------------------------------------
    // Test 6: Concurrent producer/consumer stress test.
    // ------------------------------------------------------------
    {
        constexpr std::uint64_t event_count = 1'000'000;

        SpscRingBuffer<DetectorEvent, 1024> concurrent_queue;

        std::atomic<bool> producer_done{false};

        std::uint64_t consumed = 0;

        std::thread producer([&]
                             {
            for (std::uint64_t i = 0; i < event_count; ++i) {
                DetectorEvent e{
                    .timestamp_ns = i,
                    .detector_id = 1,
                    .channel = static_cast<std::uint16_t>(i % 64),
                    .raw_energy = static_cast<std::uint32_t>(i)
                };

                while (!concurrent_queue.try_push(e)) {
                    std::this_thread::yield();
                }
            }

            producer_done.store(true, std::memory_order_release); });

        std::thread consumer([&]
                             {
            DetectorEvent e{};

            while (true) {
                if (concurrent_queue.try_pop(e)) {
                    assert(e.timestamp_ns == consumed);
                    ++consumed;
                    continue;
                }

                if (producer_done.load(std::memory_order_acquire)) {
                    break;
                }

                std::this_thread::yield();
            } });

        producer.join();
        consumer.join();

        assert(consumed == event_count);
    }

    return 0;
}