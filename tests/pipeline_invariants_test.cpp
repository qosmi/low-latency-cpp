#include <cassert>

#include "petdaq/core/pipeline_invariants.hpp"

int main()
{
    petdaq::PipelineStatistics statistics;

    assert(
        petdaq::pipeline_accounting_is_consistent(statistics));

    statistics.generated.store(
        100,
        std::memory_order_relaxed);

    statistics.enqueued.store(
        80,
        std::memory_order_relaxed);

    statistics.dropped.store(
        20,
        std::memory_order_relaxed);

    statistics.processed.store(
        80,
        std::memory_order_relaxed);

    assert(
        petdaq::pipeline_accounting_is_consistent(statistics));

    // One event disappeared between enqueue and processing.
    statistics.processed.store(
        79,
        std::memory_order_relaxed);

    assert(
        !petdaq::pipeline_accounting_is_consistent(statistics));

    // Restore processing count, then introduce an
    // accounting mismatch between generated/enqueued/dropped.
    statistics.processed.store(
        80,
        std::memory_order_relaxed);

    statistics.dropped.store(
        19,
        std::memory_order_relaxed);

    assert(
        !petdaq::pipeline_accounting_is_consistent(statistics));

    return 0;
}