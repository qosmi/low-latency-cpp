#pragma once

#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/processing/event_batch.hpp"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace petdaq
{
    /*
    fill batch
        ├── pop event
        ├── pop event
        └── ...
              │
              ▼
    process batch
        ├── process event
        ├── process event
        └── ...
    */
    template <std::size_t QueueCapacity>
    class EventProcessor final
    {
    public:
        using Queue = SpscRingBuffer<DetectorEvent, QueueCapacity>;

        static constexpr std::size_t MaxBatchSize = 64;

        EventProcessor(
            Queue &queue,
            PipelineStatistics &statistics,
            const std::atomic<bool> &producer_done,
            std::size_t batch_size = 1U)
            : queue_(queue),
              statistics_(statistics),
              producer_done_(producer_done),
              batch_size_(batch_size == 0U
                              ? 1U
                          : batch_size > MaxBatchSize
                              ? MaxBatchSize
                              : batch_size)
        {
        }

        void run()
        {
            EventBatch<MaxBatchSize> batch;

            while (true)
            {
                batch.clear();

                fill_batch(batch);

                if (!batch.empty())
                {
                    process_batch(batch);
                    continue;
                }

                if (producer_done_.load(std::memory_order_acquire))
                {
                    break;
                }

                std::this_thread::yield();
            }
        }

    private:
        void fill_batch(EventBatch<MaxBatchSize> &batch)
        {
            // batching policy: Process when either the configured batch size is reached or the queue temporarily has no more events
            while (batch.size() < batch_size_)
            {
                DetectorEvent event{};

                if (!queue_.try_pop(event))
                {
                    break;
                }

                const bool inserted = batch.push(event);

                // batch.size() < batch capacity guarantees this.
                (void)inserted;
            }
        }

        void process_batch(const EventBatch<MaxBatchSize> &batch) noexcept
        {
            for (std::size_t i = 0; i < batch.size(); ++i)
            {
                process_one(batch[i]);
            }

            statistics_.processed.fetch_add(
                batch.size(),
                std::memory_order_relaxed);
        }

        void process_one(const DetectorEvent &event) noexcept
        {
            // Placeholder for the actual processing stage.
            //
            // We deliberately keep the work small here so that later
            // benchmarks can isolate the effect of batching.

            sink_ ^= static_cast<std::uint64_t>(event.raw_energy) + event.timestamp_ns + event.detector_id + event.channel;
        }

        Queue &queue_;
        PipelineStatistics &statistics_;
        const std::atomic<bool> &producer_done_;

        std::size_t batch_size_;

        std::uint64_t sink_{0};
    };

} // namespace petdaq