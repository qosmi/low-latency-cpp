#include <iostream>

#include "petdaq/core/detector_event.hpp"
#include "petdaq/processing/calibration.hpp"
#include "petdaq/processing/event_batch.hpp"
#include "petdaq/reconstruction/image_writer.hpp"
#include "petdaq/reconstruction/reconstructor.hpp"

int main()
{
    constexpr std::size_t Width = 64;
    constexpr std::size_t Height = 64;
    constexpr std::size_t BatchCapacity = 64;

    petdaq::CalibrationTable calibration;

    petdaq::EventBatch<BatchCapacity> batch;

    for (std::uint16_t channel = 0;
         channel < Width;
         ++channel) {

        const petdaq::DetectorEvent event{
            .timestamp_ns = channel,
            .detector_id =
                static_cast<std::uint16_t>(
                    channel % Height),
            .channel = channel,
            .raw_energy =
                static_cast<std::uint32_t>(
                    100 + channel)
        };

        if (!batch.push(event)) {
            break;
        }
    }

    petdaq::Reconstructor<
        Width,
        Height,
        BatchCapacity> reconstructor;

    reconstructor.reset();

    reconstructor.process_batch(
        batch,
        calibration);

    const bool written =
        petdaq::write_image_csv(
            reconstructor.image(),
            "reconstruction.csv");

    if (!written) {
        std::cerr
            << "Failed to write reconstruction\n";
        return 1;
    }

    std::cout
        << "Projected events: "
        << reconstructor.projected_events()
        << '\n';

    std::cout
        << "Wrote reconstruction.csv\n";

    return 0;
}