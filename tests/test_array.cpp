#include <gtest/gtest.h>
#include <quartz/array.hpp>
#include <quartz/memory.hpp>
#include <utility>

namespace
{

struct alignas(64) composite_type // NOLINT (for overaligned value 64)
{
    [[maybe_unused]] qz::s32 a, b, c;
};

} // namespace

//

TEST(QzArray, Array_Initialization)
{
    // normal initialization
    {
        [[maybe_unused]] qz::array<int, 4> array1{};
        [[maybe_unused]] qz::array<composite_type, 4> array2{};

        [[maybe_unused]] constexpr qz::array<int, 10> array3{};
        [[maybe_unused]] constexpr qz::array<composite_type, 10> array4{};
    }
    // value initialization with type deduction
    {
        [[maybe_unused]] qz::array array1{0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
        [[maybe_unused]] qz::array array2{
            composite_type{.a = 1, .b = 2},
            composite_type{.a = 3, .b = 4},
            composite_type{.a = 1, .b = 2},
            composite_type{.a = 3, .b = 4},
            composite_type{.a = 1, .b = 3},
        };

        [[maybe_unused]] constexpr qz::array array3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        [[maybe_unused]] constexpr qz::array array4{
            composite_type{.a = 1,  .b = 2},
            composite_type{.a = 3,  .b = 4},
            composite_type{.a = 5,  .b = 6},
            composite_type{.a = 7,  .b = 8},
            composite_type{.a = 9, .b = 10},
        };
    }
    // zero size array
    {
        constexpr qz::array<int, 0> array{};
    }
}

TEST(QzArray, Array_Values)
{
    // normal array value checks
    {
        qz::array array = {0, 1, 2, 3};
        EXPECT_NE(array.data(), nullptr);
        EXPECT_TRUE(!array.empty());
        EXPECT_EQ(array.size(), 4);
        EXPECT_TRUE(array[0] == 0 && array[1] == 1 && array[2] == 2 && array[3] == 3);
    }
    // constexpr array value checks
    {
        constexpr qz::array array = {0, 1, 2, 3};
        static_assert(array.data() != nullptr, "Array data may not be nullptr.");
        static_assert(!array.empty(), "Value array may not be empty.");
        static_assert(array.size() == 4, "Expected 4 elements in array.");
        static_assert(array[0] == 0 && array[1] == 1 && array[2] == 2 && array[3] == 3, "Values are not as expected.");
    }
    // empty array value checks
    {
        constexpr qz::array<int, 0> array{};
        static_assert(array.data() == nullptr, "Empty array may not point to valid data.");
        static_assert(array.empty(), "Empty array must return true on call to array::empty().");
        static_assert(array.size() == 0, "Empty array must have zero elements in it.");
        // constexpr auto cannot_dereference = array[0]; /* fails to compile (as expected) */
        EXPECT_DEATH(auto cannot_dereference = array[0], "");
    }
}

TEST(QzArray, Iteration)
{
    // iterators & for loop checks
    {
        qz::array array{0, 1, 2, 3, 4};
        auto expected_value = 0;
        auto expected_match = 0;
        // classic iterator for loop
        for (auto i = array.begin(); i != array.end(); ++i) // NOLINT
        {
            if (*i == expected_value)
            {
                ++expected_match;
            }
            ++expected_value;
        }
        EXPECT_EQ(expected_match, array.size()); // all values matched

        expected_value = 0;
        expected_match = 0;
        // range based for loop
        for (auto value : array)
        {
            if (value == expected_value)
            {
                ++expected_match;
            }
            ++expected_value;
        }
        EXPECT_EQ(expected_match, array.size()); // all values matched
    }
    //
    {
        constexpr qz::array<int, 0> array{};
        static_assert(array.begin() == array.end(), "Begin and end iterators must be equal for empty arrays.");
    }
}

TEST(QzArray, Fill_Swap)
{
    qz::array array_a{1, 1, 1, 1};
    qz::array array_b{2, 2, 2, 2};

    array_a.swap(array_b);

    auto match_count = 0;
    for (auto value : array_a)
    {
        match_count += static_cast<int>(value == 2);
    }
    for (auto value : array_b)
    {
        match_count += static_cast<int>(value == 1);
    }

    EXPECT_EQ(match_count, array_a.size() + array_b.size()); // values of both arrays matched.

    //

    array_a.fill(3);
    match_count = 0;
    for (auto value : array_a)
    {
        match_count += static_cast<int>(value == 3);
    }

    EXPECT_EQ(match_count, array_a.size()); // all values matched.
}
