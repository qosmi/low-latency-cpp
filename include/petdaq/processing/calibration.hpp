#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace petdaq
{
    struct CalibrationConstants final
    {
        float pedestal{};
        float gain{1.0F};
    };

    class CalibrationTable final
    {
    public:
        static constexpr std::size_t channel_count = 64;

        CalibrationTable()
        {
            for (auto &constants : constants_)
            {
                constants.pedestal = 0.0F;
                constants.gain = 1.0F;
            }
        }

        void set(
            std::uint16_t channel,
            CalibrationConstants constants)
        {
            if (channel >= channel_count)
            {
                throw std::out_of_range("invalid detector channel");
            }

            constants_[channel] = constants;
        }

        [[nodiscard]]
        CalibrationConstants get(std::uint16_t channel) const
        {
            if (channel >= channel_count)
            {
                throw std::out_of_range("invalid detector channel");
            }

            return constants_[channel];
        }

        [[nodiscard]]
        float calibrate(
            std::uint16_t channel,
            std::uint32_t raw_energy) const
        {
            const CalibrationConstants constants = get(channel);
            const float raw = static_cast<float>(raw_energy);
            return (raw - constants.pedestal) * constants.gain;
        }

    private:
        std::array<CalibrationConstants, channel_count> constants_{};
    };

    struct CalibratedEvent final
    {
        std::uint64_t timestamp_ns{};
        std::uint16_t detector_id{};
        std::uint16_t channel{};
        float energy{};
    };

} // namespace petdaq