#pragma once

#include <cstdint>
#include <array>

struct CalibrationConstants
{
    double pedestal{};
    double gain{1.0};
};

[[nodiscard]]
double calibrate(
    std::uint32_t raw,
    const CalibrationConstants &c) noexcept
{
    return (static_cast<double>(raw) - c.pedestal) * c.gain;
}

std::array<CalibrationConstants, 64> calibration;