#pragma once

/**
 * @defgroup QzMacros Helper macros
 * @brief A collection of helper macros and defines. Include <quartz/macros.hpp> to use them.
 * @{
 */

/**
 * @defgroup QzDebugMacros Debug helper macros
 * @ingroup QzMacros
 * @brief Macros for helping with debugging.
 */

/**
 * @ingroup QzDebugMacros
 * @brief Macro which expands to the file name of where it is placed.
 */
#define QZ_FILE __FILE__

/**
 * @ingroup QzDebugMacros
 * @brief Macro which expands to the function name and signature of where this is placed.
 */
#if defined(_MSC_VER)
    #define QZ_FUNC __FUNCSIG__
#else
    #define QZ_FUNC __PRETTY_FUNCTION__
#endif

/**
 * @ingroup QzDebugMacros
 * @brief Macro which expands to the line number of where this is placed.
 */
#define QZ_LINE __LINE__

//

/**
 * @defgroup QzUtilityMacros Utility macros.
 * @brief General utility/helper macros
 */

/**
 * @ingroup QzUtilityMacros
 * @brief Concatenate two raw tokens.
 */
#define QZ_CONCAT(a, b) QZ_CONCAT_(a, b)
/// @cond
#define QZ_CONCAT_(a, b) a##b
/// @endcond

/**
 * @ingroup QzUtilityMacros
 * @brief Convert the arguments to a single C-style multibyte string.
 */
#define QZ_STRINGIFY(...) QZ_STRINGIFY_(__VA_ARGS__)
/// @cond
#define QZ_STRINGIFY_(...) #__VA_ARGS__
/// @endcond

/**
 * @ingroup QzUtilityMacros
 * @brief Convert the arguments to a single wide char string.
 */
#define QZ_STRINGIFY_W(...) QZ_CONCAT(L, QZ_STRINGIFY(__VA_ARGS__))

/** @} */
