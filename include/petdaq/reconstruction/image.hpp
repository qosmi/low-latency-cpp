#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace petdaq {

template <std::size_t Width, std::size_t Height>
class Image2D final {
public:
    using Pixel = float;

    void clear() noexcept
    {
        pixels_.fill(0.0F);
    }

    [[nodiscard]] constexpr std::size_t width() const noexcept
    {
        return Width;
    }

    [[nodiscard]] constexpr std::size_t height() const noexcept
    {
        return Height;
    }

    [[nodiscard]] Pixel& at(
        std::size_t x,
        std::size_t y) noexcept
    {
        return pixels_[y * Width + x];
    }

    [[nodiscard]] const Pixel& at(
        std::size_t x,
        std::size_t y) const noexcept
    {
        return pixels_[y * Width + x];
    }

    [[nodiscard]] const Pixel* data() const noexcept
    {
        return pixels_.data();
    }

private:
    std::array<Pixel, Width * Height> pixels_{};
};

} // namespace petdaq