#pragma once

#include "petdaq/core/statistics.hpp"

namespace petdaq {

[[nodiscard]] inline bool
pipeline_accounting_is_consistent(
    const PipelineStatistics& statistics) noexcept
{
    const auto generated =
        statistics.generated.load(std::memory_order_relaxed);

    const auto enqueued =
        statistics.enqueued.load(std::memory_order_relaxed);

    const auto dropped =
        statistics.dropped.load(std::memory_order_relaxed);

    const auto processed =
        statistics.processed.load(std::memory_order_relaxed);

    return generated == enqueued + dropped
        && processed == enqueued;
}

} // namespace petdaq