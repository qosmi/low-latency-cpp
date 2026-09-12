#pragma once

#include "petdaq/processing/calibration.hpp"
#include "petdaq/reconstruction/image.hpp"

#include <cstddef>

namespace petdaq {

template <std::size_t Width, std::size_t Height>
class EventProjector final {
public:
    bool project(
        const CalibratedEvent& event,
        Image2D<Width, Height>& image) const noexcept
    {
        const auto x =
            static_cast<std::size_t>(event.channel);

        const auto y =
            static_cast<std::size_t>(event.detector_id);

        if (x >= Width || y >= Height) {
            return false;
        }

        image.at(x, y) += event.energy;

        return true;
    }
};

} // namespace petdaq