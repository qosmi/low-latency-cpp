#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace petdaq
{

    class LatencyStatistics final
    {
    public:
        void add(std::uint64_t latency_ns)
        {
            samples_.push_back(latency_ns);
        }

        void sort()
        {
            std::sort(samples_.begin(), samples_.end());
        }

        [[nodiscard]]
        std::uint64_t percentile(double p) const
        {
            if (samples_.empty())
            {
                return 0;
            }

            const auto index = static_cast<std::size_t>(
                p * static_cast<double>(samples_.size() - 1));

            return samples_[index];
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return samples_.size();
        }

    private:
        std::vector<std::uint64_t> samples_;
    };

}