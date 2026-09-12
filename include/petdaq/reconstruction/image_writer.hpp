#pragma once

#include "petdaq/reconstruction/image.hpp"
#include <cstddef>
#include <iomanip>
#include <fstream>
#include <string>

namespace petdaq {

template <std::size_t Width, std::size_t Height>
bool write_image_csv(
    const Image2D<Width, Height>& image,
    const std::string& filename)
{
    std::ofstream output(filename);

    if (!output) {
        return false;
    }

    output << std::setprecision(9);

    if (!output) {
        return false;
    }

    for (std::size_t y = 0; y < Height; ++y) {
        for (std::size_t x = 0; x < Width; ++x) {
            if (x != 0) {
                output << ',';
            }

            output << image.at(x, y);
        }

        output << '\n';
    }

    return true;
}

} // namespace petdaq