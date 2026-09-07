#pragma once

#include <atomic>
#include <cstdint>

namespace petdaq {

/**
 * Counters shared between the producer and consumer.
 *
 * These counters are intentionally simple. They are observability data rather
 * than part of the acquisition protocol, so relaxed atomics are sufficient:
 * we need atomicity, but no ordering relationship between these counters.
 */
struct PipelineStatistics final {
    std::atomic<std::uint64_t> generated{0};
    std::atomic<std::uint64_t> enqueued{0};
    std::atomic<std::uint64_t> dropped{0};
    std::atomic<std::uint64_t> processed{0};
};

} // namespace petdaq
