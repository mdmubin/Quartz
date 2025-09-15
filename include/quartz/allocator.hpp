#pragma once

#include <new>
#include <limits>
#include <type_traits>

#include "quartz/types.hpp"

namespace qz
{

/// @defgroup QzAllocator Memory allocators
/// @brief Support for different memory allocation strategies. Although currently only the global allocator exists,
/// other allocators will soon be added.
///
/// @{

///
/// @brief The default allocator class which uses the global operators new and delete to allocate and deallocate
/// objects.
/// @details The allocations are sufficiently aligned, and this allocator also supports allocating over-aligned types.
/// This is a general purpose allocator, which is used as the default allocator throughout the containers in @ref
/// QzContainers.
///
/// @tparam T The type of value being allocated.
///
template <class T>
struct allocator
{
    // ASSERTIONS

    static_assert(!std::is_const_v<T> && !std::is_volatile_v<T>, "T may not be a const or volatile for qz::allocator.");

    // TYPEDEFS

    using value_type = T;
    using pointer    = value_type *;
    using size_type  = usz;

    // CTOR

    /// @brief Ctor.
    constexpr allocator() noexcept = default;

    /// @brief Rebinding Ctor.
    template <class U>
    constexpr allocator(const allocator<U> & /* unused */) noexcept
    {
        // empty
    }

    // STATIC METHODS

    /// @brief The maximum number of element that may be allocated.
    [[nodiscard]] static constexpr size_type max_size() noexcept
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    // METHODS

    /// @brief Allocate a chunk of memory that is capable of holding the given number of elements.
    /// @param count The number of elements that needs to fit into the chunk.
    /// @return A pointer to the allocated chunk. The pointer is naturally aligned.
    /// @throws std::bad_alloc on allocation failure.
    /// @throws std::bad_array_new_length on count greater than max_size().
    [[nodiscard]] pointer allocate(size_type count)
    {
        if (count > max_size())
        {
            throw std::bad_array_new_length();
        }
        return static_cast<pointer>(::operator new(count * sizeof(T), std::align_val_t{alignof(T)}));
    }

    /// @brief Allocate a chunk of overaligned memory that is capable of holding the given number of elements.
    /// @param count The number of elements that needs to fit into the chunk.
    /// @param alignment The alignment requirement. It is expected to be a power of 2.
    /// @return A pointer to the allocated chunk. The pointer is sufficiently aligned to the given value.
    /// @throws std::bad_alloc on allocation failure.
    /// @throws std::bad_array_new_length on count greater than max_size().
    [[nodiscard]] pointer allocate(size_type count, size_type alignment)
    {
        if (count > max_size())
        {
            throw std::bad_array_new_length();
        }
        alignment = alignment < alignof(T) ? alignof(T) : alignment;
        return static_cast<pointer>(::operator new(count * sizeof(T), std::align_val_t{alignment}));
    }

    /// @brief Deallocate a chunk of memory that was allocated by this allocator.
    /// @param ptr Pointer to the chunk.
    /// @param count The number of elements in the chunk.
    void deallocate(pointer ptr, size_type count)
    {
        ::operator delete(ptr, count * sizeof(T), std::align_val_t{alignof(T)});
    }

    /// @brief Deallocate a chunk of overaligned memory that was allocated by this allocator.
    /// @param ptr Pointer to the chunk.
    /// @param count The number of elements in the chunk.
    /// @param align The alignment requirement which was used to allocate the chunk.
    void deallocate(pointer ptr, size_type count, size_type align)
    {
        ::operator delete(ptr, count * sizeof(T), std::align_val_t{align});
    }
};

///
/// @brief Equality operator.
/// @return Always true.
///
template <class T, class U>
constexpr bool operator==(const allocator<T> & /* unused */, const allocator<U> & /* unused */) noexcept
{
    return true;
}

///
/// @brief Non-equal operator.
/// @return Always false.
///
template <class T, class U>
constexpr bool operator!=(const allocator<T> &lhs, const allocator<U> &rhs) noexcept
{
    return !(lhs == rhs);
}

///
/// @}
///

} // namespace qz
