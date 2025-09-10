#pragma once

#include "quartz/macros.hpp"
#include "quartz/types.hpp"

namespace qz
{

/**
 * @defgroup QzAssert Assertion handling
 * @brief Functions and macros for handling assertions and reporting assertion failures.
 *
 * @{
 */

/**
 * @brief Contextual data describing the reasons for the assertion failure.
 */
struct assertion_failure_context
{
    /**
     * @brief The condition which caused the assertion failure.
     */
    const char *cnd;
    /**
     * @brief The message describing the failure.
     */
    const char *msg;
    /**
     * @brief The file in which the failure occurred.
     */
    const char *file;
    /**
     * @brief The function in which the failure occurred.
     */
    const char *func;
    /**
     * @brief The line number in which the failure occurred.
     */
    usz line;
    /**
     * @brief Pointer to additional user data. A value of nullptr signifies no additional user data.
     */
    void *user_data;
};

//

/**
 * @brief A typedef for the assertion reporter callback function. This function is responsible for notifying the user of
 * the details of an assertion failure.
 */
using assertion_reporter = void (*)(const assertion_failure_context &context);

/**
 * @brief Retrieve the current global assertion reporter.
 * @return The default assertion reporter.
 */
assertion_reporter get_default_assertion_reporter();
/**
 * @brief Update the global assertion reporter to the new one provided.
 * @param reporter The new reporter.
 *
 * @return The old assertion reporter.
 */
assertion_reporter set_default_assertion_reporter(assertion_reporter reporter);

//

/**
 * @brief Call the global assertion reporter and then terminates the program by calling `std::abort()`.
 * @param context Contextual information about the assertion failure.
 * @note This function does not return.
 */
[[noreturn]]
void handle_assertion_failure(const assertion_failure_context &context);

/** @} */

} // namespace qz

#ifndef NDEBUG
/**
 * @ingroup QzAssert
 * @brief Assert that a condition holds true. Report failure and terminate if assertion fails.
 * @param cnd The condition required to be true.
 * @note This macro expands to a no-op in release builds.
 */
#define QZ_ASSERT(cnd)                                                                                                 \
    ((cnd) ? static_cast<void>(0) : qz::handle_assertion_failure({#cnd, "", QZ_FILE, QZ_FUNC, QZ_LINE}))
/**
 * @ingroup QzAssert
 * @brief Assert that a condition holds true. Report failure with a message and terminate if assertion fails.
 * @param cnd The condition required to be true.
 * @param msg The additional message to be displayed on assertion failure.
 * @note This macro expands to a no-op in release builds.
 */
#define QZ_ASSERT_MSG(cnd, msg)                                                                                        \
    ((cnd) ? static_cast<void>(0) : qz::handle_assertion_failure({#cnd, msg, QZ_FILE, QZ_FUNC, QZ_LINE}))
#else
    #define QZ_ASSERT(...) static_cast<void>(0)
    #define QZ_ASSERT_MSG(...) static_cast<void>(0)
#endif

/**
 * @ingroup QzAssert
 * @brief Verify that a condition holds true. Report failure and terminate if verification fails.
 * @param cnd The condition required to be true.
 * @note This macro performs the verification even in release builds.
 */
#define QZ_VERIFY(cnd)                                                                                                 \
    ((cnd) ? static_cast<void>(0) : qz::handle_assertion_failure({#cnd, "", QZ_FILE, QZ_FUNC, QZ_LINE}))
/**
 * @ingroup QzAssert
 * @brief Verify that a condition holds true. Report failure with a message and terminate if verification fails.
 * @param cnd The condition required to be true.
 * @param msg The additional message to be displayed on assertion failure.
 * @note This macro performs the verification even in release builds.
 */
#define QZ_VERIFY_MSG(cnd, msg)                                                                                        \
    ((cnd) ? static_cast<void>(0) : qz::handle_assertion_failure({#cnd, msg, QZ_FILE, QZ_FUNC, QZ_LINE}))
