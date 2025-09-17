#pragma once

#include <type_traits>

#include "quartz/atomic_operations.hpp"
#include "quartz/types.hpp"

namespace qz
{

//

template <class T>
class atomic_base
{
  private:

    template <class U>
    struct atomic_storage_traits
    {
        static constexpr usz size = sizeof(U) == 1  ? 1
                                  : sizeof(U) == 2  ? 2
                                  : sizeof(U) <= 4  ? 4
                                  : sizeof(U) <= 8  ? 8
                                  : sizeof(U) <= 16 ? 16
                                  : sizeof(U);
        static constexpr usz padding = size - sizeof(U);
    };

    //

    template <class U, bool = atomic_storage_traits<U>::padding != 0>
    struct atomic_storage
    {
        alignas(atomic_storage_traits<U>::size) U data;
        u8 padding[atomic_storage_traits<U>::padding];
    };
    template <class U>
    struct atomic_storage<U, false>
    {
        alignas(atomic_storage_traits<U>::size) U data;
    };

  public:
    // TYPEDEFS

    using store_type = atomic_storage<T>;
    using operations = atomic_operations<store_type>;

    // CTOR

    atomic_base() noexcept = default;

    constexpr atomic_base(T desired) noexcept : m_storage{desired} {}

    // DELETED

    atomic_base(const atomic_base &)                     = delete;
    atomic_base &operator=(const atomic_base &)          = delete;
    atomic_base &operator=(const atomic_base &) volatile = delete;

    // METHODS

    [[nodiscard]] bool is_lock_free() const noexcept
    {
        return atomic_operations_are_lock_free<store_type>();
    }
    [[nodiscard]] bool is_lock_free() const volatile noexcept
    {
        return atomic_operations_are_lock_free<store_type>();
    }

    void store(T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        operations::store(m_storage, {desired}, order);
    }
    void store(T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        operations::store(const_cast<atomic_base *>(this)->m_storage, {desired}, order); // const casting is fine here
    }

    [[nodiscard]] T load(memory_order order = memory_order_seq_cst) const noexcept
    {
        return operations::load(const_cast<atomic_base *>(this)->m_storage, order).data;
    }
    [[nodiscard]] T load(memory_order order = memory_order_seq_cst) const volatile noexcept
    {
        return operations::load(const_cast<atomic_base *>(this)->m_storage, order).data;
    }

    T exchange(T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        return operations::exchange(m_storage, {desired}, order).data;
    }
    T exchange(T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return operations::exchange(const_cast<atomic_base *>(this)->m_storage, {desired}, order).data;
    }

    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) noexcept
    {
        auto exp_res = store_type{expected};
        auto result = operations::cmpxchg(m_storage, exp_res, {desired}, true, success, failure);
        expected    = exp_res.data;
        return result;
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        auto exp_res = store_type{expected};
        auto result = operations::cmpxchg(const_cast<atomic_base *>(this)->m_storage, exp_res, {desired}, true, success, failure);
        expected = exp_res.data;
        return result;
    }

    bool compare_exchange_weak(T &expected, T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        if (order == memory_order_release)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_relaxed);
        }
        if (order == memory_order_acq_rel)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_acquire);
        }
        return compare_exchange_weak(expected, desired, order, order);
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        if (order == memory_order_release)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_relaxed);
        }
        if (order == memory_order_acq_rel)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_acquire);
        }
        return compare_exchange_weak(expected, desired, order, order);
    }

    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) noexcept
    {
        auto exp_res = store_type{expected};
        auto result = operations::cmpxchg(m_storage, exp_res, {desired}, false, success, failure);
        expected    = exp_res.data;
        return result;
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        auto exp_res = store_type{expected};
        auto result = operations::cmpxchg(const_cast<atomic_base *>(this)->m_storage, exp_res, {desired}, false, success, failure);
        expected = exp_res.data;
        return result;
    }

    bool compare_exchange_strong(T &expected, T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        if (order == memory_order_release)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_relaxed);
        }
        if (order == memory_order_acq_rel)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_acquire);
        }
        return compare_exchange_weak(expected, desired, order, order);
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        if (order == memory_order_release)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_relaxed);
        }
        if (order == memory_order_acq_rel)
        {
            return compare_exchange_weak(expected, desired, order, memory_order_acquire);
        }
        return compare_exchange_weak(expected, desired, order, order);
    }

    // PUBLIC MEMBERS

    static constexpr bool is_always_lock_free = atomic_operations_are_always_lock_free<store_type>();

  protected:
    // PROTECTED MEMBERS

    store_type m_storage;
};

template <class T>
class atomic : public atomic_base<T>
{
  public:
    // TYPEDEFS

    using value_type = T;
    using base_type  = atomic_base<value_type>;

    // CTOR

    atomic() noexcept = default;

    constexpr atomic(value_type desired) noexcept : base_type{desired} {}

    // DELETED

    atomic(const atomic &)                     = delete;
    atomic &operator=(const atomic &)          = delete;
    atomic &operator=(const atomic &) volatile = delete;

    // OPERATORS

    operator value_type() const noexcept
    {
        return base_type::load();
    }
    operator value_type() const volatile noexcept
    {
        return base_type::load();
    }

    value_type operator=(value_type desired) noexcept
    {
        base_type::store(desired);
        return desired;
    }
    value_type operator=(value_type desired) volatile noexcept
    {
        base_type::store(desired);
        return desired;
    }

    // ASSERTIONS

    static_assert(
        std::is_trivially_copyable_v<T> &&
        std::is_copy_constructible_v<T> &&
        std::is_move_constructible_v<T> &&
        std::is_copy_assignable_v<T>    &&
        std::is_move_assignable_v<T>    &&
        std::is_same_v<T, std::remove_cv_t<T>>,
        "Given type `T` does not satisfy the requirements for atomic<T>."
    );
};

template <typename t>
class atomic<t *> : public atomic_base<t *>
{
  public:
    // TYPEDEFS

    using value_type      = t;
    using pointer_type    = value_type *;
    using base_type       = atomic_base<pointer_type>;
    using operations      = atomic_operations<pointer_type>;
    using difference_type = ssz;

    // CTOR

    atomic() noexcept = default;

    constexpr atomic(pointer_type desired) noexcept : base_type{desired} {}

    // DELETED

    atomic(const atomic &)                     = delete;
    atomic &operator=(const atomic &)          = delete;
    atomic &operator=(const atomic &) volatile = delete;

    // METHODS

    pointer_type fetch_add(difference_type value, memory_order order = memory_order_seq_cst) noexcept
    {
        return operations::fetch_add(base_type::m_storage.data, value * sizeof(value_type), order);
    }
    pointer_type fetch_add(difference_type value, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return operations::fetch_add(const_cast<atomic *>(this)->m_storage.data, value * sizeof(value_type), order);
    }

    pointer_type fetch_sub(difference_type value, memory_order order = memory_order_seq_cst) noexcept
    {
        return operations::fetch_sub(base_type::m_storage.data, value * sizeof(value_type), order);
    }
    pointer_type fetch_sub(difference_type value, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return operations::fetch_sub(const_cast<atomic *>(this)->m_storage.data, value * sizeof(value_type), order);
    }

    // OPERATORS

    operator pointer_type() const          noexcept { return base_type::load(); }
    operator pointer_type() const volatile noexcept { return base_type::load(); }

    pointer_type operator=(pointer_type desired)          noexcept { base_type::store(desired); return desired; }
    pointer_type operator=(pointer_type desired) volatile noexcept { base_type::store(desired); return desired; }

    pointer_type operator++()             noexcept { return fetch_add(1) + 1; }
    pointer_type operator++()    volatile noexcept { return fetch_add(1) + 1; }
    pointer_type operator++(int)          noexcept { return fetch_add(1) + 1; }
    pointer_type operator++(int) volatile noexcept { return fetch_add(1) + 1; }
    pointer_type operator--()             noexcept { return fetch_sub(1) - 1; }
    pointer_type operator--()    volatile noexcept { return fetch_sub(1) - 1; }
    pointer_type operator--(int)          noexcept { return fetch_sub(1) - 1; }
    pointer_type operator--(int) volatile noexcept { return fetch_sub(1) - 1; }
    pointer_type operator+=(difference_type value)          noexcept { return fetch_add(value) + value; }
    pointer_type operator+=(difference_type value) volatile noexcept { return fetch_add(value) + value; }
    pointer_type operator-=(difference_type value)          noexcept { return fetch_sub(value) - value; }
    pointer_type operator-=(difference_type value) volatile noexcept { return fetch_sub(value) - value; }
};

//


#define QZ_ATOMIC_INTEGRAL_TYPE(T)                                                                                     \
    template <>                                                                                                        \
    class atomic<T> : public atomic_base<T>                                                                            \
    {                                                                                                                  \
      public:                                                                                                          \
        /* TYPEDEFS */                                                                                                 \
                                                                                                                       \
        using value_type      = T;                                                                                     \
        using base_type       = atomic_base<value_type>;                                                               \
        using operations      = atomic_operations<value_type>;                                                         \
        using difference_type = value_type;                                                                            \
                                                                                                                       \
        /* CTORS */                                                                                                    \
                                                                                                                       \
        atomic() noexcept = default;                                                                                   \
                                                                                                                       \
        constexpr atomic(value_type desired) noexcept : base_type{desired} {}                                          \
                                                                                                                       \
        /* DELETED */                                                                                                  \
                                                                                                                       \
        atomic(const atomic &)                     = delete;                                                           \
        atomic &operator=(const atomic &)          = delete;                                                           \
        atomic &operator=(const atomic &) volatile = delete;                                                           \
                                                                                                                       \
        /* METHODS */                                                                                                  \
                                                                                                                       \
        value_type fetch_add(value_type value, memory_order order = memory_order_seq_cst) noexcept                     \
        {                                                                                                              \
            return operations::fetch_add(base_type::m_storage.data, value, order);                                     \
        }                                                                                                              \
        value_type fetch_add(value_type value, memory_order order = memory_order_seq_cst) volatile noexcept            \
        {                                                                                                              \
            return operations::fetch_add(const_cast<atomic *>(this)->m_storage.data, value, order);                    \
        }                                                                                                              \
                                                                                                                       \
        value_type fetch_sub(value_type value, memory_order order = memory_order_seq_cst) noexcept                     \
        {                                                                                                              \
            return operations::fetch_sub(base_type::m_storage.data, value, order);                                     \
        }                                                                                                              \
        value_type fetch_sub(value_type value, memory_order order = memory_order_seq_cst) volatile noexcept            \
        {                                                                                                              \
            return operations::fetch_sub(const_cast<atomic *>(this)->m_storage.data, value, order);                    \
        }                                                                                                              \
                                                                                                                       \
        value_type fetch_and(value_type value, memory_order order = memory_order_seq_cst) noexcept                     \
        {                                                                                                              \
            return operations::fetch_and(base_type::m_storage.data, value, order);                                     \
        }                                                                                                              \
        value_type fetch_and(value_type value, memory_order order = memory_order_seq_cst) volatile noexcept            \
        {                                                                                                              \
            return operations::fetch_and(const_cast<atomic *>(this)->m_storage.data, value, order);                    \
        }                                                                                                              \
                                                                                                                       \
        value_type fetch_or(value_type value, memory_order order = memory_order_seq_cst) noexcept                      \
        {                                                                                                              \
            return operations::fetch_or(base_type::m_storage.data, value, order);                                      \
        }                                                                                                              \
        value_type fetch_or(value_type value, memory_order order = memory_order_seq_cst) volatile noexcept             \
        {                                                                                                              \
            return operations::fetch_or(const_cast<atomic *>(this)->m_storage.data, value, order);                     \
        }                                                                                                              \
                                                                                                                       \
        value_type fetch_xor(value_type value, memory_order order = memory_order_seq_cst) noexcept                     \
        {                                                                                                              \
            return operations::fetch_xor(base_type::m_storage.data, value, order);                                     \
        }                                                                                                              \
        value_type fetch_xor(value_type value, memory_order order = memory_order_seq_cst) volatile noexcept            \
        {                                                                                                              \
            return operations::fetch_xor(const_cast<atomic *>(this)->m_storage.data, value, order);                    \
        }                                                                                                              \
                                                                                                                       \
        /* OPERATORS */                                                                                                \
                                                                                                                       \
        operator value_type() const          noexcept { return base_type::load(); }                                    \
        operator value_type() const volatile noexcept { return base_type::load(); }                                    \
                                                                                                                       \
        value_type operator=(value_type desired)          noexcept { base_type::store(desired); return desired; }      \
        value_type operator=(value_type desired) volatile noexcept { base_type::store(desired); return desired; }      \
                                                                                                                       \
        value_type operator++()             noexcept { return fetch_add(1) + 1; }                                      \
        value_type operator++()    volatile noexcept { return fetch_add(1) + 1; }                                      \
        value_type operator++(int)          noexcept { return fetch_add(1) + 1; }                                      \
        value_type operator++(int) volatile noexcept { return fetch_add(1) + 1; }                                      \
        value_type operator--()             noexcept { return fetch_sub(1) - 1; }                                      \
        value_type operator--()    volatile noexcept { return fetch_sub(1) - 1; }                                      \
        value_type operator--(int)          noexcept { return fetch_sub(1) - 1; }                                      \
        value_type operator--(int) volatile noexcept { return fetch_sub(1) - 1; }                                      \
                                                                                                                       \
        value_type operator+=(value_type value)          noexcept { return fetch_add(value) + value; }                 \
        value_type operator+=(value_type value) volatile noexcept { return fetch_add(value) + value; }                 \
        value_type operator-=(value_type value)          noexcept { return fetch_sub(value) - value; }                 \
        value_type operator-=(value_type value) volatile noexcept { return fetch_sub(value) - value; }                 \
        value_type operator&=(value_type value)          noexcept { return fetch_and(value) & value; }                 \
        value_type operator&=(value_type value) volatile noexcept { return fetch_and(value) & value; }                 \
        value_type operator|=(value_type value)          noexcept { return fetch_or (value) | value; }                 \
        value_type operator|=(value_type value) volatile noexcept { return fetch_or (value) | value; }                 \
        value_type operator^=(value_type value)          noexcept { return fetch_xor(value) ^ value; }                 \
        value_type operator^=(value_type value) volatile noexcept { return fetch_xor(value) ^ value; }                 \
    }


QZ_ATOMIC_INTEGRAL_TYPE(char);
QZ_ATOMIC_INTEGRAL_TYPE(short);
QZ_ATOMIC_INTEGRAL_TYPE(int);
QZ_ATOMIC_INTEGRAL_TYPE(long);
QZ_ATOMIC_INTEGRAL_TYPE(long long);
QZ_ATOMIC_INTEGRAL_TYPE(unsigned char);
QZ_ATOMIC_INTEGRAL_TYPE(unsigned short);
QZ_ATOMIC_INTEGRAL_TYPE(unsigned int);
QZ_ATOMIC_INTEGRAL_TYPE(unsigned long);
QZ_ATOMIC_INTEGRAL_TYPE(unsigned long long);
QZ_ATOMIC_INTEGRAL_TYPE(signed char);
QZ_ATOMIC_INTEGRAL_TYPE(char16_t);
QZ_ATOMIC_INTEGRAL_TYPE(char32_t);
QZ_ATOMIC_INTEGRAL_TYPE(wchar_t);

// we don't need this anymore
#undef QZ_ATOMIC_INTEGRAL_TYPE

//

using atomic_s8  = atomic<s8>;
using atomic_s16 = atomic<s16>;
using atomic_s32 = atomic<s32>;
using atomic_s64 = atomic<s64>;
using atomic_ssz = atomic<ssz>;
using atomic_usz = atomic<usz>;
using atomic_u8  = atomic<u8>;
using atomic_u16 = atomic<u16>;
using atomic_u32 = atomic<u32>;
using atomic_u64 = atomic<u64>;

using atomic_bool = atomic<bool>;
using atomic_wchar = atomic<wchar_t>;
using atomic_char16 = atomic<char16_t>;
using atomic_char32 = atomic<char32_t>;

//

class atomic_flag
{
  public:
    /* TYPEDEFS */
    using this_type = atomic_flag;

    // CTORS
    atomic_flag() noexcept = default;

    // DELETED

    atomic_flag(const this_type &)                   = delete;
    this_type &operator=(const this_type &)          = delete;
    this_type &operator=(const this_type &) volatile = delete;

    // METHODS

    void clear(memory_order order = memory_order_seq_cst) noexcept
    {
        m_flag.store(false, order);
    }
    void clear(memory_order order = memory_order_seq_cst) volatile noexcept
    {
        m_flag.store(false, order);
    }

    bool test_and_set(memory_order order = memory_order_seq_cst) noexcept
    {
        return m_flag.exchange(true, order);
    }
    bool test_and_set(memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return m_flag.exchange(true, order);
    }

    [[nodiscard]] bool test(memory_order order = memory_order_seq_cst) const noexcept
    {
        return m_flag.load(order);
    }
    [[nodiscard]] bool test(memory_order order = memory_order_seq_cst) const volatile noexcept
    {
        return m_flag.load(order);
    }

  private:
    // MEMBERS

    atomic_bool m_flag;
};

} // namespace qz
