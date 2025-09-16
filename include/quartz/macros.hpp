#pragma once

///
/// @defgroup QzMacros Helper macros
/// @brief A collection of helper macros and defines. Include <quartz/macros.hpp> to use them.
/// @{
///

#if defined(__linux) || defined(__linux__)
    /// @ingroup QzPlatformMacros
    /// @brief Defined if the current platform is linux.
    #define QZ_PLATFORM_LINUX 1
    /// @ingroup QzPlatformMacros
    /// @brief Platform name defined if the current platform is linux.
    #define QZ_PLATFORM_STRING "linux"
#elif defined(_WIN32) || defined(_WIN64)
    /// @ingroup QzPlatformMacros
    /// @brief Defined if the current platform is windows.
    #define QZ_PLATFORM_WINDOWS 1
    /// @ingroup QzPlatformMacros
    /// @brief Platform name defined if the current platform is windows.
    #define QZ_PLATFORM_STRING "windows"
#else
    #error unrecognized platform
#endif

#if defined(QZ_PLATFORM_LINUX)
    #if defined(__clang__)
        #define QZ_COMPILER_CLANG 1
        #define QZ_COMPILER_STRING "clang"
        #define QZ_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
    #elif defined(__GNUC__)
        #define QZ_COMPILER_GCC 1
        #define QZ_COMPILER_STRING "gcc"
        #define QZ_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
    #else
        #error unrecognized compiler.
    #endif
#elif defined(QZ_PLATFORM_WINDOWS)
    #if defined(__clang__)
        #define QZ_COMPILER_CLANG 1
        #if defined(_MSC_VER)
            #define QZ_COMPILER_CLANG_CL 1
            #define QZ_COMPILER_STRING "clang-cl"
        #elif defined(__MINGW32__) || defined(__MINGW64__)
            #define QZ_COMPILER_MINGW_CLANG 1
            #define QZ_COMPILER_STRING "mingw-clang"
        #else
            #define QZ_COMPILER_STRING "clang"
        #endif
        #define QZ_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
    #elif defined(_MSC_VER)
        #define QZ_COMPILER_MSVC 1
        #define QZ_COMPILER_STRING "msvc"
        #define QZ_COMPILER_VERSION _MSC_VER
    #elif defined(__GNUC__)
        #define QZ_COMPILER_GCC 1
        #if defined(__MINGW32__) || defined(__MINGW64__)
            #define QZ_COMPILER_MINGW_GCC 1
            #define QZ_COMPILER_STRING "mingw-gcc"
        #else
            #define QZ_COMPILER_STRING "gcc"
        #endif
        #define QZ_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
    #else
        #error unrecognized compiler.
    #endif
#else
    #error cannot determine compiler on unrecognized platform.
#endif

#if defined(QZ_PLATFORM_LINUX)
    #if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
        #define QZ_ARCHITECTURE_X64 1
        #define QZ_ARCHITECTURE_STRING "x64"
    #else
        #error unrecognized platform architecture.
    #endif
#elif defined(QZ_PLATFORM_WINDOWS)
    #if defined(_M_X64) || defined(_M_AMD64)
        #define QZ_ARCHITECTURE_X64 1
        #define QZ_ARCHITECTURE_STRING "x64"
    #else
        #error unrecognized platform architecture.
    #endif
#else
    #error cannot determine architecture on unrecognized platform.
#endif

///
/// @defgroup QzDebugMacros Debug helper macros
/// @ingroup QzMacros
/// @brief Macros for helping with debugging.
///

///
/// @ingroup QzDebugMacros
/// @brief Macro which expands to the file name of where it is placed.
///
#define QZ_FILE __FILE__

///
/// @ingroup QzDebugMacros
/// @brief Macro which expands to the function name and signature of where this is placed.
///
#if defined(_MSC_VER)
    #define QZ_FUNC __FUNCSIG__
#else
    #define QZ_FUNC __PRETTY_FUNCTION__
#endif

///
/// @ingroup QzDebugMacros
/// @brief Macro which expands to the line number of where this is placed.
///
#define QZ_LINE __LINE__

//

///
/// @defgroup QzUtilityMacros Utility macros.
/// @brief General utility/helper macros
///

///
/// @ingroup QzUtilityMacros
/// @brief Concatenate two raw tokens.
///
#define QZ_CONCAT(a, b) QZ_CONCAT_1(a, b)
/// @cond Undocumented
#define QZ_CONCAT_1(a, b) a##b
/// @endcond

///
/// @ingroup QzUtilityMacros
/// @brief Convert the arguments to a single C-style multibyte string.
///
#define QZ_STRINGIFY(...) QZ_STRINGIFY_1(__VA_ARGS__)
/// @cond Undocumented
#define QZ_STRINGIFY_1(...) #__VA_ARGS__
/// @endcond

///
/// @ingroup QzUtilityMacros
/// @brief Convert the arguments to a single wide char string.
///
#define QZ_STRINGIFY_W(...) QZ_CONCAT(L, QZ_STRINGIFY(__VA_ARGS__))

///
/// @ingroup QzUtilityMacros
/// @brief Check if the compiler has a certain builtin function.
///
#if defined(__has_builtin)
    #define QZ_HAS_BUILTIN(X) __has_builtin(X)
#else
    #define QZ_HAS_BUILTIN(X) 0
#endif

///
/// @ingroup QzUtilityMacros
/// @brief Mark a certain branch of code as unreachable/unlikely to be reached.
///
#if defined(QZ_COMPILER_MSVC)
    #define QZ_UNREACHABLE() __assume(false)
#elif QZ_HAS_BUILTIN(__builtin_unreachable)
    #define QZ_UNREACHABLE() __builtin_unreachable()
#endif

///
/// @ingroup QzUtilityMacros
/// @brief Mark a certain variable or statement as unused.
///
#define QZ_UNUSED(X) static_cast<void>(X)

///
/// @}
///
