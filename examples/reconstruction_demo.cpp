#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include "petdaq/core/detector_event.hpp"
#include "petdaq/processing/calibration.hpp"
#include "petdaq/processing/event_batch.hpp"
#include "petdaq/reconstruction/image_writer.hpp"
#include "petdaq/reconstruction/reconstruction_config.hpp"
#include "petdaq/reconstruction/reconstructor.hpp"

namespace {

constexpr std::uint32_t RawEnergy = 100;

double gaussian(
    double x,
    double y,
    double center_x,
    double center_y,
    double sigma) noexcept
{
    const double dx = x - center_x;
    const double dy = y - center_y;

    const double distance_squared =
        dx * dx + dy * dy;

    const double denominator =
        2.0 * sigma * sigma;

    return std::exp(
        -distance_squared / denominator);
}

double phantom_intensity(
    std::size_t x,
    std::size_t y) noexcept
{
    const double px =
        static_cast<double>(x);

    const double py =
        static_cast<double>(y);

    // Two synthetic hot regions.
    const double left_hotspot =
        5.0 * gaussian(
            px,
            py,
            18.0,
            32.0,
            4.0);

    const double right_hotspot =
        4.0 * gaussian(
            px,
            py,
            46.0,
            32.0,
            5.0);

    // A synthetic ring-like structure.
    const double dx = px - 32.0;
    const double dy = py - 32.0;

    const double radius =
        std::sqrt(dx * dx + dy * dy);

    const double ring_distance =
        radius - 20.0;

    const double ring =
        1.5 * std::exp(
            -(ring_distance * ring_distance)
            / (2.0 * 1.5 * 1.5));

    return left_hotspot
         + right_hotspot
         + ring;
}

} // namespace

int main()
{
    petdaq::CalibrationTable calibration;

    using Config = petdaq::ReconstructionConfig;

    petdaq::Reconstructor<
        Config::width,
        Config::height,
        Config::batch_capacity> reconstructor;

    reconstructor.reset();

    petdaq::EventBatch<Config::batch_capacity> batch;

    std::uint64_t timestamp = 0;

    std::size_t generated_events = 0;

    for (std::size_t y = 0; y < Config::height; ++y) {
        for (std::size_t x = 0; x < Config::width; ++x) {
            const double intensity =
                phantom_intensity(x, y);

            const auto event_count =
                static_cast<std::size_t>(
                    intensity * 20.0);

            for (std::size_t i = 0;
                 i < event_count;
                 ++i) {

                const petdaq::DetectorEvent event{
                    .timestamp_ns = timestamp++,
                    .detector_id =
                        static_cast<std::uint16_t>(y),
                    .channel =
                        static_cast<std::uint16_t>(x),
                    .raw_energy = RawEnergy
                };

                if (!batch.push(event)) {
                    reconstructor.process_batch(
                        batch,
                        calibration);

                    batch.clear();

                    const bool pushed =
                        batch.push(event);

                    if (!pushed) {
                        std::cerr
                            << "Failed to add event to batch\n";
                        return 1;
                    }
                }

                ++generated_events;
            }
        }
    }

    // Process the final partially filled batch.
    if (!batch.empty()) {
        reconstructor.process_batch(
            batch,
            calibration);
    }

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
        << "Synthetic reconstruction\n"
        << "-------------------------\n"
        << "Generated events: "
        << generated_events
        << '\n'
        << "Projected events: "
        << reconstructor.projected_events()
        << '\n'
        << "Image size: "
        << Config::width
        << " x "
        << Config::height
        << '\n'
        << "Wrote reconstruction.csv\n";

    return 0;
}