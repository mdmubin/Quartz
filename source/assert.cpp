#include <cstdio>
#include <cstdlib>

#include "quartz/assert.hpp"
#include "quartz/atomic.hpp"

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

qz::atomic g_assertion_reporter = default_assertion_reporter;

} // namespace

qz::assertion_reporter_fn qz::get_default_assertion_reporter()
{;
    return g_assertion_reporter.load();
}

qz::assertion_reporter_fn qz::set_default_assertion_reporter(assertion_reporter_fn reporter)
{
    return g_assertion_reporter.exchange(reporter);
}

void qz::handle_assertion_failure(const assertion_failure_context &context)
{
    g_assertion_reporter.load()(context);
    std::abort();
}
