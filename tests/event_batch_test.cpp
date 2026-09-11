#include <cassert>
#include <cstdint>
#include "petdaq/core/detector_event.hpp"
#include "petdaq/processing/event_batch.hpp"

int main()
{
    using petdaq::DetectorEvent;
    using petdaq::EventBatch;

    EventBatch<4> batch;

    // Newly created batch is empty.
    assert(batch.empty());
    assert(batch.size() == 0);

    // Add four events.
    for (std::uint64_t i = 0; i < 4; ++i)
    {
        DetectorEvent event{
            .timestamp_ns = i,
            .detector_id = 1,
            .channel = static_cast<std::uint16_t>(i),
            .raw_energy = static_cast<std::uint32_t>(100 + i)};

        assert(batch.push(event));
    }

    // Batch is now full.
    assert(batch.size() == 4);
    assert(!batch.push(DetectorEvent{}));

    // Verify ordering and data.
    for (std::uint64_t i = 0; i < 4; ++i)
    {
        assert(batch[i].timestamp_ns == i);
        assert(batch[i].channel == i);
        assert(batch[i].raw_energy == 100 + i);
    }

    // Clear the batch.
    batch.clear();

    assert(batch.empty());
    assert(batch.size() == 0);

    return 0;
}