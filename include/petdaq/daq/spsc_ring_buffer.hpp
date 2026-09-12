#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace petdaq {

/**
 * A bounded single-producer/single-consumer ring buffer.
 *
 * Ownership model:
 *   - exactly one thread calls try_push()
 *   - exactly one thread calls try_pop()
 *
 * This restriction is what allows us to avoid a mutex. The buffer is bounded,
 * so it also never allocates memory while the pipeline is running.
 *
 * Capacity must be a power of two. That lets the index wrap with a cheap
 * bit-mask instead of a modulo operation.
 */
template <typename T, std::size_t Capacity>
class SpscRingBuffer final {
    static_assert(Capacity >= 2, "Capacity must be at least 2.");
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be a power of two.");
    static_assert(std::is_trivially_copyable_v<T>,
                  "The first implementation uses trivially-copyable events.");

public:
    SpscRingBuffer() = default;

    SpscRingBuffer(const SpscRingBuffer&) = delete;
    SpscRingBuffer& operator=(const SpscRingBuffer&) = delete;

    /**
     * Attempts to enqueue one item.
     *
     * The producer uses relaxed access to its own write index. The acquire
     * load of the read index observes consumer progress. The release store
     * publishes the newly written element to the consumer.
     */
    [[nodiscard]] bool try_push(const T& value) noexcept {
        const auto write = write_index_.load(std::memory_order_relaxed);
        const auto read = read_index_.load(std::memory_order_acquire);

        if (write - read == Capacity) {
            return false; // Buffer is full.
        }

        storage_[write & mask_] = value;

        // Publish the element only after writing its contents.
        write_index_.store(write + 1, std::memory_order_release);
        return true;
    }

    /**
     * Attempts to dequeue one item.
     *
     * The acquire load synchronizes with the producer's release store, making
     * the element contents visible before we read them.
     */
    [[nodiscard]] bool try_pop(T& value) noexcept {
        const auto read = read_index_.load(std::memory_order_relaxed);
        const auto write = write_index_.load(std::memory_order_acquire);

        if (read == write) {
            return false; // Buffer is empty.
        }

        value = storage_[read & mask_];

        // Publish consumer progress so the producer may reuse this slot.
        read_index_.store(read + 1, std::memory_order_release);
        return true;
    }

    std::size_t try_pop_bulk(
        T* output,
        std::size_t max_count) noexcept
    {
        if (max_count == 0) {
            return 0;
        }

        const auto read =
            read_index_.load(std::memory_order_relaxed);

        const auto write =
            write_index_.load(std::memory_order_acquire);

        const auto available = write - read;

        const auto count =
            available < max_count
                ? static_cast<std::size_t>(available)
                : max_count;

        for (std::size_t i = 0; i < count; ++i) {
            output[i] =
                storage_[(read + i) & mask_];
        }

        read_index_.store(
            read + count,
            std::memory_order_release);

        return count;
    }

private:
    static constexpr std::size_t mask_ = Capacity - 1;

    // Separate cache lines reduce the chance of false sharing between the
    // producer-owned and consumer-owned indices.
    alignas(64) std::atomic<std::size_t> write_index_{0};
    alignas(64) std::atomic<std::size_t> read_index_{0};

    std::array<T, Capacity> storage_{};
};

} // namespace petdaq
