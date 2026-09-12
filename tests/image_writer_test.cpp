#include <cassert>
#include <fstream>
#include <string>

#include "petdaq/reconstruction/image.hpp"
#include "petdaq/reconstruction/image_writer.hpp"

int main()
{
    petdaq::Image2D<4, 3> image;

    image.clear();

    image.at(1, 0) = 10.0F;
    image.at(2, 1) = 20.0F;
    image.at(3, 2) = 30.0F;

    const std::string filename =
        "pet_daq_test_image.csv";

    assert(
        petdaq::write_image_csv(
            image,
            filename));

    std::ifstream input(filename);

    assert(input);

    std::string line;

    assert(std::getline(input, line));
    assert(line == "0,10,0,0");

    assert(std::getline(input, line));
    assert(line == "0,0,20,0");

    assert(std::getline(input, line));
    assert(line == "0,0,0,30");

    return 0;
}