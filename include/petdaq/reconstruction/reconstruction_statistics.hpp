#pragma once

#include <cstddef>

namespace petdaq {

struct ReconstructionStatistics final {
    std::size_t received_events{0};
    std::size_t projected_events{0};
    std::size_t rejected_events{0};
};

} // namespace petdaq