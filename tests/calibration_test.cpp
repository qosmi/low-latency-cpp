#include <cassert>
#include <cmath>
#include "petdaq/processing/calibration.hpp"

int main()
{
    using petdaq::CalibrationConstants;
    using petdaq::CalibrationTable;

    CalibrationTable table;
    // Default calibration.
    assert(table.calibrate(0, 100) == 100.0F);

    // Pedestal subtraction.
    table.set(
        3,
        CalibrationConstants{
            .pedestal = 100.0F,
            .gain = 1.0F});
    assert(table.calibrate(3, 100) == 0.0F);

    // Gain.
    table.set(
        4,
        CalibrationConstants{
            .pedestal = 100.0F,
            .gain = 2.0F});
    assert(table.calibrate(4, 200) == 200.0F);

    // Invalid channel.
    bool threw = false;
    try
    {
        (void)table.calibrate(64, 100);
    }
    catch (const std::out_of_range &)
    {
        threw = true;
    }

    assert(threw);
    return 0;
}