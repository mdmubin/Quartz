#include <gtest/gtest.h>
#include <quartz/allocator.hpp>

namespace
{

class alignas(64) overaligned_type
{
    qz::s32 m_value;
};

} // namespace

TEST(QzAllocator, Allocate_Deallocate_Checks)
{
    auto alloc_a = qz::allocator<int>{};
    auto alloc_b = qz::allocator<overaligned_type>{};

    // test normal allocation / deallocation
    {
        const auto alloc_size = 1024;

        auto *ptr_a = alloc_a.allocate(alloc_size);
        // should successfully allocate, or throw exception.
        EXPECT_NE(ptr_a, nullptr);
        alloc_a.deallocate(ptr_a, alloc_size);

        auto *ptr_b = alloc_b.allocate(alloc_size);
        EXPECT_NE(ptr_b, nullptr);
        // over aligned type should be aligned properly.
        EXPECT_TRUE(reinterpret_cast<qz::usz>(ptr_b) % alignof(overaligned_type) == 0); // NOLINT (reinterpret cast)
        alloc_b.deallocate(ptr_b, alloc_size);
    }

    // test overaligned allocation / deallocation
    {
        const auto alloc_size  = 1024;
        const auto alloc_align = 1024;

        auto *ptr_a = alloc_a.allocate(alloc_size, alloc_align);
        EXPECT_NE(ptr_a, nullptr);
        EXPECT_TRUE((reinterpret_cast<qz::usz>(ptr_a) % alloc_align) == 0); // NOLINT
        alloc_a.deallocate(ptr_a, alloc_size);

        // try allocate with low alignment, and check if it is sufficiently aligned.
        auto *ptr_b = alloc_b.allocate(alloc_size, 2);
        EXPECT_NE(ptr_b, nullptr);
        EXPECT_TRUE((reinterpret_cast<qz::usz>(ptr_b) % alignof(overaligned_type)) == 0); // NOLINT
        alloc_b.deallocate(ptr_b, alloc_size, alloc_align);
    }
}
