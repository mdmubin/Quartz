#include <gtest/gtest.h>
#include <quartz/assert.hpp>

TEST(QzAssert, Assertions)
{
#ifndef NDEBUG
    EXPECT_DEATH(QZ_ASSERT(false), "");
    EXPECT_DEATH(QZ_ASSERT_MSG(false, "Test failure."), "");
#endif
    EXPECT_DEATH(QZ_VERIFY(false), "");
    EXPECT_DEATH(QZ_VERIFY_MSG(false, "Test failure."), "");
}
