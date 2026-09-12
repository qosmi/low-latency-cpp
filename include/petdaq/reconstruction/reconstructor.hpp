#pragma once

#include "petdaq/processing/calibration.hpp"
#include "petdaq/reconstruction/image.hpp"
#include "petdaq/reconstruction/projector.hpp"

#include <cstddef>

/*
Reconstructor
    ├── accepts CalibratedEvent
    ├── projects event
    └── owns reconstructed Image2D
*/

namespace petdaq {

template <std::size_t Width, std::size_t Height>
class Reconstructor final {
public:
    using Image = Image2D<Width, Height>;

    void reset() noexcept
    {
        image_.clear();
        projected_events_ = 0;
    }

    bool process(
        const CalibratedEvent& event) noexcept
    {
        if (!projector_.project(event, image_)) {
            return false;
        }

        ++projected_events_;

        return true;
    }

    [[nodiscard]]
    const Image& image() const noexcept
    {
        return image_;
    }

    [[nodiscard]]
    std::size_t projected_events() const noexcept
    {
        return projected_events_;
    }

private:
    Image image_;
    EventProjector<Width, Height> projector_;
    std::size_t projected_events_{0};
};

} // namespace petdaq