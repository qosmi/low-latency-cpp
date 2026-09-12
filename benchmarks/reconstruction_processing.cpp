#include "petdaq/core/detector_event.hpp"
#include "petdaq/processing/calibration.hpp"
#include "petdaq/processing/event_batch.hpp"
#include "petdaq/reconstruction/reconstructor.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>

namespace {

constexpr std::size_t Width = 64;
constexpr std::size_t Height = 64;
constexpr std::size_t BatchCapacity = 64;

constexpr std::size_t EventCount = 5'000'000;

}

int main()
{
    using Reconstructor =
        petdaq::Reconstructor<
            Width,
            Height,
            BatchCapacity>;

    petdaq::CalibrationTable calibration;
    Reconstructor reconstructor;

    petdaq::EventBatch<BatchCapacity> batch;

    reconstructor.reset();

    std::size_t generated = 0;

    const auto start =
        std::chrono::steady_clock::now();

    while (generated < EventCount) {
        batch.clear();

        while (batch.size() < batch.capacity() && generated < EventCount) {
            const auto index =
                static_cast<std::uint64_t>(
                    generated);

            const petdaq::DetectorEvent event{
                .timestamp_ns = index,
                .detector_id =
                    static_cast<std::uint16_t>(
                        index % Height),
                .channel =
                    static_cast<std::uint16_t>(
                        (index / Height) % Width),
                .raw_energy = 100
            };

            const bool pushed =
                batch.push(event);

            if (!pushed) {
                break;
            }

            ++generated;
        }

        reconstructor.process_batch(
            batch,
            calibration);
    }

    const auto elapsed =
        std::chrono::steady_clock::now() - start;

    const double seconds =
        std::chrono::duration<double>(
            elapsed).count();

    const double throughput =
        seconds > 0.0
            ? static_cast<double>(
                  reconstructor.projected_events())
              / seconds
            : 0.0;

    std::cout
        << "PET-DAQ reconstruction benchmark\n"
        << "---------------------------------\n"
        << "Events: "
        << reconstructor.received_events()
        << '\n'
        << "Projected: "
        << reconstructor.projected_events()
        << '\n'
        << "Rejected: "
        << reconstructor.rejected_events()
        << '\n'
        << "Time: "
        << std::fixed
        << std::setprecision(2)
        << seconds * 1000.0
        << " ms\n"
        << "Throughput: "
        << std::setprecision(0)
        << throughput
        << " events/s\n";

    return 0;
}