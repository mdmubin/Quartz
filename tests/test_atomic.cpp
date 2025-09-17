#include <thread>
#include <gtest/gtest.h>
#include <quartz/atomic.hpp>

struct sample_128bits_1
{
    qz::s32 a, b, c, d;

    bool operator==(const sample_128bits_1 &other) const
    {
        return a == other.a && b == other.b && c == other.c && d == other.d;
    }
};

struct sample_128bits_2
{
    qz::s64 a, b;

    bool operator==(const sample_128bits_2 &other) const
    {
        return a == other.a && b == other.b;
    }
};

struct sample_24bits
{
    qz::s8 a, b, c;

    bool operator==(const sample_24bits &other) const
    {
        return a == other.a && b == other.b && c == other.c;
    }
};

struct sample_96bits
{
    qz::s32 a, b, c;

    bool operator==(const sample_24bits &other) const
    {
        return a == other.a && b == other.b && c == other.c;
    }
};

TEST(AtomicTest, ConstructionTest)
{
    using namespace qz;
    {
        [[maybe_unused]] atomic_s8              atomic_s8{};
        [[maybe_unused]] atomic_s16             atomic_s16{};
        [[maybe_unused]] atomic_s32             atomic_s32{};
        [[maybe_unused]] atomic_s64             atomic_s64{};
        [[maybe_unused]] atomic_u8              atomic_u8{};
        [[maybe_unused]] atomic_u16             atomic_u16{};
        [[maybe_unused]] atomic_u32             atomic_u32{};
        [[maybe_unused]] atomic_u64             atomic_u64{};
        [[maybe_unused]] atomic_wchar           atomic_wchar{};
        [[maybe_unused]] atomic_char16          atomic_char16{};
        [[maybe_unused]] atomic_char32          atomic_char32{};
        [[maybe_unused]] atomic<sample_128bits_1> atomic_sample1{};
        [[maybe_unused]] atomic<sample_128bits_2> atomic_sample2{};
    }
    {
        [[maybe_unused]] constexpr atomic_s8              atomic_s8{1};
        [[maybe_unused]] constexpr atomic_s16             atomic_s16{1};
        [[maybe_unused]] constexpr atomic_s32             atomic_s32{1};
        [[maybe_unused]] constexpr atomic_s64             atomic_s64{1};
        [[maybe_unused]] constexpr atomic_u8              atomic_u8{1};
        [[maybe_unused]] constexpr atomic_u16             atomic_u16{1};
        [[maybe_unused]] constexpr atomic_u32             atomic_u32{1};
        [[maybe_unused]] constexpr atomic_u64             atomic_u64{1};
        [[maybe_unused]] constexpr atomic_wchar           atomic_wchar{1};
        [[maybe_unused]] constexpr atomic_char16          atomic_char16{1};
        [[maybe_unused]] constexpr atomic_char32          atomic_char32{1};
        [[maybe_unused]] constexpr atomic<sample_128bits_1> atomic_sample1{
            {1, 2, 3, 4}
        };
        [[maybe_unused]] constexpr atomic<sample_128bits_2> atomic_sample2{
            {1, 2}
        };
    }
}

TEST(AtomicTest, LoadTest)
{
    using namespace qz;
    {
        constexpr atomic_s8 i{1};
        EXPECT_EQ(i.load(memory_order_relaxed), 1);
        EXPECT_EQ(i.load(memory_order_consume), 1);
        EXPECT_EQ(i.load(memory_order_acquire), 1);
        EXPECT_EQ(i.load(memory_order_seq_cst), 1);

        constexpr atomic_s8 j{5};
        EXPECT_NE(j.load(memory_order_relaxed), s8{});
        EXPECT_NE(j.load(memory_order_consume), s8{});
        EXPECT_NE(j.load(memory_order_acquire), s8{});
        EXPECT_NE(j.load(memory_order_seq_cst), s8{});

        // these generate warnings on GCC
        EXPECT_DEATH((void)i.load(memory_order_release), "Invalid memory order constraint for atomic load.");
        EXPECT_DEATH((void)i.load(memory_order_acq_rel), "Invalid memory order constraint for atomic load.");
    }
    {
        atomic x{
            sample_128bits_1{1, 2, 3, 4}
        };
        EXPECT_EQ(x.load(), (sample_128bits_1{1, 2, 3, 4}));

        atomic y{
            sample_128bits_2{5, 6}
        };
        EXPECT_EQ(y.load(), (sample_128bits_2{5, 6}));
    }
}

TEST(AtomicTest, StoreTest)
{
    using namespace qz;
    {
        atomic_s16 i{};
        i.store(1, memory_order_relaxed);
        EXPECT_EQ(i.load(), 1);
        i.store(4, memory_order_release);
        EXPECT_EQ(i.load(), 4);
        i.store(6, memory_order_seq_cst);
        EXPECT_EQ(i.load(), 6);

        // these generate warnings on GCC
        EXPECT_DEATH(i.store(2, memory_order_consume), "Invalid memory order constraint for atomic store.");
        EXPECT_DEATH(i.store(3, memory_order_acquire), "Invalid memory order constraint for atomic store.");
        EXPECT_DEATH(i.store(5, memory_order_acq_rel), "Invalid memory order constraint for atomic store.");

        atomic<sample_128bits_1> x{};
        x.store({5, 6, 7, 8});
        EXPECT_EQ(x.load(), (sample_128bits_1{5, 6, 7, 8}));

        atomic<sample_128bits_2> y{};
        y.store({4, 6});
        EXPECT_EQ(y.load(), (sample_128bits_2{4, 6}));
    }
}

TEST(AtomicTest, ExchangeTest)
{
    using namespace qz;
    {
        atomic_s32 i{INT32_MIN};
        EXPECT_EQ(i.exchange(50), INT32_MIN);
        EXPECT_EQ(i.load(), 50);
        EXPECT_EQ(i.exchange(2), 50);
        EXPECT_EQ(i.load(), 2);
        EXPECT_EQ(i.exchange(INT32_MAX), 2);
        EXPECT_EQ(i.load(), INT32_MAX);

        atomic<sample_128bits_1> x{};
        x.store({5, 6, 7, 8});
        EXPECT_EQ(x.exchange({1, 2, 3, 4}), (sample_128bits_1{5, 6, 7, 8}));
        EXPECT_EQ(x.load(), (sample_128bits_1{1, 2, 3, 4}));

        atomic<sample_128bits_2> y{};
        y.store({4, 6});
        EXPECT_EQ(y.exchange({99, 99}), (sample_128bits_2{4, 6}));
        EXPECT_EQ(y.load(), (sample_128bits_2{99, 99}));
    }
}

TEST(AtomicTest, CompareExchange)
{
    using namespace qz;
    // NOTE: On x86 systems, both compare_exchange_weak and compare_exchange_strong are implemented using a cmpxchg
    // instruction. i.e. both use a compare-and-swap operation, which is unlike other processors (RISC-V or ARM) which
    // use a Load-Linked/Store-Conditional instruction. In other words, in x86, both compare_exchange_weak and
    // compare_exchange_strong are the same.
    {
        {
            atomic_s64 i{INT64_MIN};
            s64 j = 0;
            EXPECT_FALSE(i.compare_exchange_weak(j, 1));
            EXPECT_EQ(j, INT64_MIN);
        }
        {
            atomic_s64 i{INT64_MIN};
            s64 j = 0;
            EXPECT_FALSE(i.compare_exchange_strong(j, 1));
            EXPECT_EQ(j, INT64_MIN);
        }
        {
            atomic_s64 i{INT64_MIN};
            s64 j = INT64_MIN;
            EXPECT_TRUE(i.compare_exchange_weak(j, 1));
            EXPECT_EQ(j, INT64_MIN);
            EXPECT_EQ(i.load(), 1);
        }
        {
            atomic_s64 i{INT64_MIN};
            s64 j = INT64_MIN;
            EXPECT_TRUE(i.compare_exchange_strong(j, 1));
            EXPECT_EQ(j, INT64_MIN);
            EXPECT_EQ(i.load(), 1);
        }
    }
}

TEST(AtomicTest, LockFreeTest)
{
    // ensure lock free types are the same as standard implementations.
    {
        constexpr std::atomic<qz::s64> standard{};
        constexpr qz::atomic<qz::s64> implementation{};
        EXPECT_EQ(standard.is_lock_free(), implementation.is_lock_free());
        static_assert(decltype(standard)::is_always_lock_free == decltype(implementation)::is_always_lock_free);
    }
    {
        constexpr std::atomic<sample_128bits_1> standard{};
        constexpr qz::atomic<sample_128bits_1> implementation{};
        EXPECT_EQ(standard.is_lock_free(), implementation.is_lock_free());
        static_assert(decltype(standard)::is_always_lock_free == decltype(implementation)::is_always_lock_free);
    }
    {
        constexpr std::atomic<sample_128bits_2> standard{};
        constexpr qz::atomic<sample_128bits_2> implementation{};
        EXPECT_EQ(standard.is_lock_free(), implementation.is_lock_free());
        static_assert(decltype(standard)::is_always_lock_free == decltype(implementation)::is_always_lock_free);
    }
    {
        constexpr std::atomic<sample_24bits> standard{};
        constexpr qz::atomic<sample_24bits> implementation{};
        EXPECT_EQ(standard.is_lock_free(), implementation.is_lock_free());
        static_assert(!decltype(implementation)::is_always_lock_free);
    }
    {
        constexpr std::atomic<sample_96bits> standard{};
        constexpr qz::atomic<sample_96bits> implementation{};
        EXPECT_EQ(standard.is_lock_free(), implementation.is_lock_free());
        static_assert(decltype(standard)::is_always_lock_free == decltype(implementation)::is_always_lock_free);
    }
}

TEST(AtomicTest, FetchAdd)
{
    using namespace qz;
    {
        atomic_s8 i{};
        EXPECT_EQ(i.fetch_add(1), 0);
        EXPECT_EQ(i.load(), 1);

        EXPECT_EQ(i.fetch_add(1), 1);
        EXPECT_EQ(i.load(), 2);
    }
    {
        atomic_s16 i{};
        EXPECT_EQ(i.fetch_add(1), 0);
        EXPECT_EQ(i.load(), 1);

        EXPECT_EQ(i.fetch_add(1), 1);
        EXPECT_EQ(i.load(), 2);
    }
    {
        atomic_s32 i{};
        EXPECT_EQ(i.fetch_add(1), 0);
        EXPECT_EQ(i.load(), 1);

        EXPECT_EQ(i.fetch_add(1), 1);
        EXPECT_EQ(i.load(), 2);
    }
    {
        atomic_s64 i{};
        EXPECT_EQ(i.fetch_add(1), 0);
        EXPECT_EQ(i.load(), 1);

        EXPECT_EQ(i.fetch_add(1), 1);
        EXPECT_EQ(i.load(), 2);
    }
}

TEST(AtomicTest, FetchSub)
{
    using namespace qz;
    {
        atomic_s8 i{};
        EXPECT_EQ(i.fetch_sub(1), 0);
        EXPECT_EQ(i.load(), -1);

        EXPECT_EQ(i.fetch_sub(1), -1);
        EXPECT_EQ(i.load(), -2);
    }
    {
        atomic_s16 i{};
        EXPECT_EQ(i.fetch_sub(1), 0);
        EXPECT_EQ(i.load(), -1);

        EXPECT_EQ(i.fetch_sub(1), -1);
        EXPECT_EQ(i.load(), -2);
    }
    {
        atomic_s32 i{};
        EXPECT_EQ(i.fetch_sub(1), 0);
        EXPECT_EQ(i.load(), -1);

        EXPECT_EQ(i.fetch_sub(1), -1);
        EXPECT_EQ(i.load(), -2);
    }
    {
        atomic_s64 i{};
        EXPECT_EQ(i.fetch_sub(1), 0);
        EXPECT_EQ(i.load(), -1);

        EXPECT_EQ(i.fetch_sub(1), -1);
        EXPECT_EQ(i.load(), -2);
    }
}

TEST(AtomicTest, FetchAnd)
{
    using namespace qz;
    {
        atomic_s8 i{0};
        EXPECT_EQ(i.fetch_and(1), 0);
        EXPECT_EQ(i.load(), 0);

        atomic_s8 j{1};
        EXPECT_EQ(j.fetch_and(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
    {
        atomic_s16 i{5};
        EXPECT_EQ(i.fetch_and(1), 5);
        EXPECT_EQ(i.load(), 1);

        atomic_s16 j{2};
        EXPECT_EQ(j.fetch_and(1), 2);
        EXPECT_EQ(j.load(), 0);
    }
    {
        atomic_s32 i{0xF7};
        EXPECT_EQ(i.fetch_and(0x77), 0xF7);
        EXPECT_EQ(i.load(), 0x77);

        atomic_s32 j{1};
        EXPECT_EQ(j.fetch_and(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
    {
        atomic_s64 i{0};
        EXPECT_EQ(i.fetch_and(1), 0);
        EXPECT_EQ(i.load(), 0);

        atomic_s64 j{1};
        EXPECT_EQ(j.fetch_and(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
}

TEST(AtomicTest, FetchOr)
{
    using namespace qz;
    {
        atomic_s8 i{0};
        EXPECT_EQ(i.fetch_or(1), 0);
        EXPECT_EQ(i.load(), 1);

        atomic_s8 j{1};
        EXPECT_EQ(j.fetch_or(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
    {
        atomic_s16 i{5};
        EXPECT_EQ(i.fetch_or(1), 5);
        EXPECT_EQ(i.load(), 5);

        atomic_s16 j{6};
        EXPECT_EQ(j.fetch_or(1), 6);
        EXPECT_EQ(j.load(), 7);
    }
    {
        atomic_s32 i{0xF7};
        EXPECT_EQ(i.fetch_or(0x77), 0xF7);
        EXPECT_EQ(i.load(), 0xF7);

        atomic_s32 j{0xFF};
        EXPECT_EQ(j.fetch_or(0x00), 0xFF);
        EXPECT_EQ(j.load(), 0xFF);
    }
    {
        atomic_s64 i{0};
        EXPECT_EQ(i.fetch_or(INT64_MIN), 0);
        EXPECT_EQ(i.load(), INT64_MIN);

        atomic_s64 j{1};
        EXPECT_EQ(j.fetch_or(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
}

TEST(AtomicTest, FetchXor)
{
    using namespace qz;
    {
        atomic_s8 i{0};
        EXPECT_EQ(i.fetch_xor(1), 0);
        EXPECT_EQ(i.load(), 1);

        atomic_s8 j{1};
        EXPECT_EQ(j.fetch_xor(1), 1);
        EXPECT_EQ(j.load(), 0);
    }
    {
        atomic_s16 i{5};
        EXPECT_EQ(i.fetch_xor(1), 5);
        EXPECT_EQ(i.load(), 4);

        atomic_s16 j{2};
        EXPECT_EQ(j.fetch_xor(1), 2);
        EXPECT_EQ(j.load(), 3);
    }
    {
        atomic_s32 i{0xF7};
        EXPECT_EQ(i.fetch_xor(0x77), 0xF7);
        EXPECT_EQ(i.load(), 0x80);

        atomic_s32 j{1};
        EXPECT_EQ(j.fetch_and(1), 1);
        EXPECT_EQ(j.load(), 1);
    }
    {
        atomic_s64 i{0xFF};
        EXPECT_EQ(i.fetch_xor(0xFF), 0xFF);
        EXPECT_EQ(i.load(), 0);

        atomic_s64 j{0};
        EXPECT_EQ(j.fetch_xor(0xFF), 0);
        EXPECT_EQ(j.load(), 0xFF);
    }
}

TEST(AtomicTest, IncrementMT)
{
    for (int i = 0; i < 100; i++)
    {
        qz::atomic_s32 value{};
        {
            std::thread t1{[&value] {
                for (int i = 0; i < 100000; i++)
                {
                    ++value;
                }
            }};
            std::thread t2{[&value] {
                for (int i = 0; i < 100000; i++)
                {
                    ++value;
                }
            }};

            t1.join();
            t2.join();
        }
        EXPECT_EQ(value.load(), 200000);
    }
}
