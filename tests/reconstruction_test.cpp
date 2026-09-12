#include <cassert>
#include "petdaq/processing/calibration.hpp"
#include "petdaq/reconstruction/image.hpp"
#include "petdaq/reconstruction/projector.hpp"
#include "petdaq/reconstruction/reconstructor.hpp"

int main()
{
    using petdaq::CalibratedEvent;
    using petdaq::EventProjector;
    using petdaq::Image2D;

    Image2D<64, 64> image;

    image.clear();

    assert(image.width() == 64);
    assert(image.height() == 64);

    assert(image.at(3, 7) == 0.0F);

    EventProjector<64, 64> projector;

    const CalibratedEvent first{
        .timestamp_ns = 100,
        .detector_id = 7,
        .channel = 3,
        .energy = 100.0F
    };

    assert(projector.project(first, image));

    assert(image.at(3, 7) == 100.0F);

    const CalibratedEvent second{
        .timestamp_ns = 101,
        .detector_id = 7,
        .channel = 3,
        .energy = 50.0F
    };

    assert(projector.project(second, image));

    assert(image.at(3, 7) == 150.0F);

    const CalibratedEvent outside_x{
        .timestamp_ns = 102,
        .detector_id = 7,
        .channel = 64,
        .energy = 25.0F
    };

    assert(!projector.project(outside_x, image));

    const CalibratedEvent outside_y{
        .timestamp_ns = 103,
        .detector_id = 64,
        .channel = 3,
        .energy = 25.0F
    };

    assert(!projector.project(outside_y, image));

    petdaq::Reconstructor<64, 64> reconstructor;
    reconstructor.reset();
    assert(reconstructor.process(first));
    assert(reconstructor.process(second));
    assert(reconstructor.projected_events() == 2);
    assert(reconstructor.image().at(3, 7) == 150.0F);

    petdaq::CalibrationTable calibration;
    calibration.set(
        3,
        petdaq::CalibrationConstants{
            .pedestal = 100.0F,
            .gain = 2.0F
        });

    petdaq::EventBatch<4> batch;

    const petdaq::DetectorEvent event_a{
        .timestamp_ns = 100,
        .detector_id = 7,
        .channel = 3,
        .raw_energy = 150
    };

    const petdaq::DetectorEvent event_b{
        .timestamp_ns = 101,
        .detector_id = 7,
        .channel = 3,
        .raw_energy = 200
    };

    assert(batch.push(event_a));
    assert(batch.push(event_b));

    petdaq::Reconstructor<64, 64, 4> batch_reconstructor;
    batch_reconstructor.reset();

    batch_reconstructor.process_batch(
        batch,
        calibration);

    assert(
        batch_reconstructor.projected_events() == 2);

    assert(
        batch_reconstructor.image().at(3, 7)
        == 300.0F);

    return 0;
}