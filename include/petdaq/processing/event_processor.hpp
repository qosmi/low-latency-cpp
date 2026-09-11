#pragma once

#include "petdaq/core/detector_event.hpp"
#include "petdaq/core/latency.hpp"
#include "petdaq/core/statistics.hpp"
#include "petdaq/daq/spsc_ring_buffer.hpp"
#include "petdaq/processing/calibration.hpp"
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
            LatencyRecorder *latency_recorder = nullptr,
            const CalibrationTable *calibration = nullptr,
            std::size_t batch_size = 1U)
            : queue_(queue),
              statistics_(statistics),
              producer_done_(producer_done),
              latency_recorder_(latency_recorder),
              calibration_(calibration),
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
                measure_latency(batch[i]);
                process_one(batch[i]);
            }

            statistics_.processed.fetch_add(
                batch.size(),
                std::memory_order_relaxed);
        }

        void process_one(const DetectorEvent &event) noexcept
        {
            const float energy =
                calibration_ == nullptr
                    ? static_cast<float>(event.raw_energy)
                    : calibration_->calibrate(
                          event.channel,
                          event.raw_energy);

            sink_ ^= static_cast<std::uint64_t>(energy) + event.timestamp_ns + event.detector_id + event.channel;
        }

        void measure_latency(const DetectorEvent &event) noexcept
        {
            if (latency_recorder_ == nullptr)
            {
                return;
            }

            const auto now = std::chrono::steady_clock::now();
            const auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
            if (now_ns <= 0)
            {
                return;
            }

            const auto now_ns_u64 = static_cast<std::uint64_t>(now_ns);
            if (now_ns_u64 < event.timestamp_ns)
            {
                return;
            }

            latency_recorder_->record(now_ns_u64 - event.timestamp_ns);
        }

        Queue &queue_;
        PipelineStatistics &statistics_;
        const std::atomic<bool> &producer_done_;
        LatencyRecorder *latency_recorder_;
        const CalibrationTable *calibration_;
        std::size_t batch_size_;
        std::uint64_t sink_{0};
    };

} // namespace petdaq