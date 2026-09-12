#pragma once

#include <cstddef>

namespace petdaq {

struct ReconstructionConfig final {
    static constexpr std::size_t width = 64;
    static constexpr std::size_t height = 64;
    static constexpr std::size_t batch_capacity = 64;
};

} // namespace petdaq