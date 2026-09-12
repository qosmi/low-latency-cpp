#pragma once

#include <array>
#include <cstddef>

#include "petdaq/core/detector_event.hpp"

namespace petdaq
{

    template <std::size_t Capacity>
    class EventBatch final
    {
    public:
        bool push(const DetectorEvent &event) noexcept
        {
            if (size_ >= Capacity)
            {
                return false;
            }

            events_[size_] = event;
            ++size_;

            return true;
        }

        void clear() noexcept
        {
            size_ = 0;
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return size_;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            return size_ == 0;
        }

        [[nodiscard]] DetectorEvent* data() noexcept
        {
            return events_.data();
        }

        [[nodiscard]] constexpr std::size_t capacity() const noexcept
        {
            return Capacity;
        }

        void set_size_from_bulk_pop(std::size_t size) noexcept
        {
            size_ = size <= Capacity
                ? size
                : Capacity;
        }

        [[nodiscard]]
        const DetectorEvent &operator[](std::size_t index) const noexcept
        {
            return events_[index];
        }

    private:
        std::array<DetectorEvent, Capacity> events_{}; // no allocations while filling the batch
        std::size_t size_{0};
    };

} // namespace petdaq