#pragma once

#include <type_traits>

#include "types.hpp"

namespace qz
{

///
/// @defgroup QzUtilities Utilities
/// @brief Collection of miscellaneous utility functions and classes.
///
/// @{
///

//

///
/// @brief Convert the given value to an r-value reference, enabling it to be moved without copying.
/// @tparam T The type of value.
/// @param value The value being moved.
///
template <class T>
[[nodiscard]] constexpr std::remove_reference_t<T> &&move(T &&value) noexcept /* NOLINT */
{
    return static_cast<std::remove_reference_t<T> &&>(value);
}

//

///
/// @brief Swap two given values.
/// @tparam T The type of the values.
/// @param valueA The first value.
/// @param valueB The second value.
///
template <class T>
constexpr void swap(T &valueA, T &valueB)
    noexcept(std::is_nothrow_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
    requires(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
{
    auto tmp = qz::move(valueA);
    valueA   = qz::move(valueB);
    valueB   = qz::move(tmp);
}

//

///
/// @brief Swap the contents of two C-style arrays.
/// @tparam T The value type of the array.
/// @tparam N The length of the array.
/// @param arrayA The first array.
/// @param arrayB The second array.
///
template <class T, usz N>
constexpr void swap(T (&arrayA)[N], T (&arrayB)[N]) noexcept(std::is_nothrow_swappable_v<T>) /* NOLINT */
    requires(std::is_swappable_v<T>)
{
    for (auto i = 0; i < N; ++i)
    {
        qz::swap(arrayA[i], arrayB[i]);
    }
}

//

///
/// @brief Get the address of the given value, even when operator& is overloaded.
/// @tparam T The type of the value.
/// @return Address of the value.
///
template <class T>
constexpr T *addressof(T &value) noexcept
{
    // __builtin_addressof is a compiler intrinsic. This function cannot be constexpr without compiler support.
    return __builtin_addressof(value);
}

///
/// @brief Disallow taking address of const RValue types.
///
template <class T>
const T *addressof(const T &&value) = delete;

/// @}

} // namespace qz
