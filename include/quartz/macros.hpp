#pragma once

/**
 * @defgroup QzMacros Helper macros
 * @brief A collection of helper macros and defines.
 * @{
 */

/**
 * @brief Macro which expands to the file name of where it is placed.
 */
#define QZ_FILE __FILE__

/**
 * @brief Macro which expands to the function name and signature of where this is placed.
 */
#if defined(_MSC_VER)
    #define QZ_FUNC __FUNCSIG__
#else
    #define QZ_FUNC __PRETTY_FUNCTION__
#endif

/**
 * @brief Macro which expands to the line number of where this is placed.
 */
#define QZ_LINE __LINE__

/** @} */
