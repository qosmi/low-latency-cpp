#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace petdaq
{

    class LatencyRecorder final
    {
    public:
        static constexpr std::size_t Capacity = 8192;

        void record(std::uint64_t latency_ns) noexcept
        {
            if (sample_count_ < Capacity)
            {
                samples_[sample_count_] = latency_ns;
                ++sample_count_;
            }

            if (latency_ns < minimum_ns_)
            {
                minimum_ns_ = latency_ns;
            }

            if (latency_ns > maximum_ns_)
            {
                maximum_ns_ = latency_ns;
            }

            total_ns_ += latency_ns;
            ++count_;
        }

        [[nodiscard]] std::uint64_t count() const noexcept
        {
            return count_;
        }

        [[nodiscard]] std::size_t sample_count() const noexcept
        {
            return sample_count_;
        }

        [[nodiscard]] std::uint64_t minimum_ns() const noexcept
        {
            return count_ == 0 ? 0 : minimum_ns_;
        }

        [[nodiscard]] std::uint64_t maximum_ns() const noexcept
        {
            return count_ == 0 ? 0 : maximum_ns_;
        }

        [[nodiscard]] std::uint64_t average_ns() const noexcept
        {
            if (count_ == 0)
            {
                return 0;
            }

            return total_ns_ / count_;
        }

        [[nodiscard]] const std::uint64_t &operator[](
            std::size_t index) const noexcept
        {
            return samples_[index];
        }

        [[nodiscard]] std::uint64_t percentile(double percentile) const noexcept
        {
            if (sample_count_ == 0)
            {
                return 0;
            }

            if (percentile <= 0.0)
            {
                return minimum_ns();
            }

            if (percentile >= 100.0)
            {
                return maximum_ns();
            }

            std::array<std::uint64_t, Capacity> sorted = samples_;

            std::sort(
                sorted.begin(),
                sorted.begin() + sample_count_);

            const double rank =
                (percentile / 100.0) * static_cast<double>(sample_count_ - 1);

            const auto index =
                static_cast<std::size_t>(rank);

            return sorted[index];
        }

    private:
        std::array<std::uint64_t, Capacity> samples_{};

        std::size_t sample_count_{0};

        std::uint64_t count_{0};
        std::uint64_t total_ns_{0};

        std::uint64_t minimum_ns_{
            std::numeric_limits<std::uint64_t>::max()};

        std::uint64_t maximum_ns_{0};
    };

} // namespace petdaq