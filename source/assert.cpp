#include "quartz/assert.hpp"

#include <cstdio>
#include <cstdlib>

namespace /* anonymous namespace */
{

void default_assertion_reporter(const qz::assertion_failure_context &context)
{
    std::fprintf(stderr,
                 "[FATAL ERROR] Assertion failure! %s\n\t"
                 "The following condition failed: %s\n\t"
                 "In file: %s:%llu inside the function: %s\n",
                 context.msg, context.cnd, context.file, context.line, context.func);
}

qz::assertion_reporter g_assertion_reporter = default_assertion_reporter;

} // namespace

qz::assertion_reporter qz::get_default_assertion_reporter()
{;
    return g_assertion_reporter;
}

qz::assertion_reporter qz::set_default_assertion_reporter(assertion_reporter reporter)
{
    auto previous_reporter = g_assertion_reporter;
    g_assertion_reporter   = reporter;
    return previous_reporter;
}

void qz::handle_assertion_failure(const assertion_failure_context &context)
{
    g_assertion_reporter(context);
    std::abort();
}
