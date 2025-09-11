#pragma once

namespace qz
{

///
/// @defgroup QzTypes Fixed-width types
///
/// @brief Integer and floating point typedefs used in the qz namespace.
/// @details Contains typedefs for fixed width integer and floating point types. Include <quartz/types.hpp> to use them.
///
/// @{
///

///
/// @brief Signed 8-bit integer type.
///
using s8 = signed char;
///
/// @brief Signed 16-bit integer type.
///
using s16 = short;
///
/// @brief Signed 32-bit integer type.
///
using s32 = int;
///
/// @brief Signed 64-bit integer type.
///
using s64 = long long;
///
/// @brief Unsigned 8-bit integer type.
///
using u8 = unsigned char;
///
/// @brief Unsigned 16-bit integer type.
///
using u16 = unsigned short;
///
/// @brief Unsigned 32-bit integer type.
///
using u32 = unsigned int;
///
/// @brief Unsigned 64-bit integer type.
///
using u64 = unsigned long long;
///
/// @brief 32-bit floating point type.
///
using f32 = float;
///
/// @brief 64-bit floating point type.
///
using f64 = double;
///
/// @brief The signed integral size type. Should be equivalent to `std::ptrdiff_t`.
///
using ssz = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
///
 /// @brief The unsigned integral size type. Should be equivalent to `std::size_t`.
 ///
using usz = decltype(sizeof(0));

///
/// @}
///

} // namespace qz
