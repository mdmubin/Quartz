#pragma once

#include "quartz/assert.hpp"
#include "quartz/macros.hpp"
#include "quartz/types.hpp"
#include "quartz/utilities.hpp"

#if defined(QZ_COMPILER_MSVC)
    #include <intrin.h>
    #define QZ_COMPILER_BARRIER() _ReadWriteBarrier() // deprecated, but what to use instead?
#else
    #define QZ_COMPILER_BARRIER() __asm__ __volatile__("" ::: "memory")
#endif

// clang-format off

#define QZ_VERIFY_ATOMIC_LOAD_ORDER(order) QZ_VERIFY_MSG(                                                              \
        (order <= memory_order_seq_cst &&                                                                              \
         order != memory_order_release &&                                                                              \
         order != memory_order_acq_rel ),                                                                              \
        "Invalid memory order constraint for atomic load."                                                             \
    )
#define QZ_VERIFY_ATOMIC_STORE_ORDER(order) QZ_VERIFY_MSG(                                                             \
        (order <= memory_order_seq_cst &&                                                                              \
         order != memory_order_consume &&                                                                              \
         order != memory_order_acquire &&                                                                              \
         order != memory_order_acq_rel),                                                                               \
         "Invalid memory order constraint for atomic store."                                                           \
    )
#define QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure) QZ_VERIFY_MSG(                                                \
        (failure <= success              &&                                                                            \
         success <= memory_order_seq_cst &&                                                                            \
         failure != memory_order_release &&                                                                            \
         failure != memory_order_acq_rel ),                                                                            \
         "Invalid memory order constraints for atomic compare exchange."                                               \
    )
#define QZ_PLACE_ATOMIC_LOAD_BARRIER(order)                                                                            \
    do {                                                                                                               \
        switch (order) {                                                                                               \
        case memory_order_consume:                                                                                     \
        case memory_order_acquire:                                                                                     \
        case memory_order_seq_cst: QZ_COMPILER_BARRIER(); break;                                                       \
        default: /* release/acq_rel pruned by QZ_VERIFY_ATOMIC_LOAD_BARRIER, and no barrier for relax */ break;        \
        }                                                                                                              \
    } while (0)

// clang-format on

namespace qz
{

enum memory_order : u8
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst,
};

template <class T, usz = sizeof(T)>
struct atomic_operations
{
    // clang-format off
    static_assert(
        sizeof(T) == 1 ||
        sizeof(T) == 2 ||
        sizeof(T) == 4 ||
        sizeof(T) == 8 ||
        sizeof(T) == 16,
        "Atomic operations are only supported if sizeof(T) is 1, 2, 4, 8 or 16 bytes."
    );
    // clang-format on
};

template <class T>
struct atomic_operations<T, 1>
{
    [[nodiscard]] static T load(T &storage, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_LOAD_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        u8 value = *addressof(reinterpret_cast<volatile u8 &>(storage));
        QZ_PLACE_ATOMIC_LOAD_BARRIER(order); // NOLINT (do-while)
#else
        u8 value = __atomic_load_n(addressof(reinterpret_cast<volatile u8 &>(storage)), order);
#endif
        return reinterpret_cast<T &>(value);
    }

    static void store(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_STORE_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        switch (order)
        {
        case memory_order_relaxed:
            *addressof(reinterpret_cast<volatile u8 &>(storage)) = reinterpret_cast<u8 &>(desired);
            break;
        case memory_order_release:
            QZ_COMPILER_BARRIER();
            *addressof(reinterpret_cast<volatile u8 &>(storage)) = reinterpret_cast<u8 &>(desired);
            break;
        case memory_order_seq_cst:
            _InterlockedExchange8(addressof(reinterpret_cast<volatile char &>(storage)),
                                  reinterpret_cast<char &>(desired));
            break;
        default:
            QZ_UNREACHABLE();
        }
#else
        __atomic_store_n(addressof(reinterpret_cast<volatile u8 &>(storage)), reinterpret_cast<u8 &>(desired), order);
#endif
    }

    static T exchange(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic exchange.");
#if defined(QZ_COMPILER_MSVC)
        char prev = _InterlockedExchange8(addressof(reinterpret_cast<volatile char &>(storage)),
                                          reinterpret_cast<char &>(desired));
#else
        u8 prev = __atomic_exchange_n(addressof(reinterpret_cast<volatile u8 &>(storage)),
                                      reinterpret_cast<u8 &>(desired), order);
#endif
        return reinterpret_cast<T &>(prev);
    }

    static bool cmpxchg(T &storage, T &expected, T desired, bool weak, memory_order success,
                        memory_order failure) noexcept
    {
        QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure);
#if defined(QZ_COMPILER_MSVC)
        static_cast<void>(weak); // only use strong cmpxchg.

        char required = *addressof(reinterpret_cast<volatile char &>(expected));
        char previous = _InterlockedCompareExchange8(addressof(reinterpret_cast<volatile char &>(storage)),
                                                     reinterpret_cast<char &>(desired), required);
        if (previous == required)
        {
            return true;
        }
        reinterpret_cast<char &>(expected) = previous;
        return false;
#else
        return __atomic_compare_exchange_n(addressof(reinterpret_cast<volatile u8 &>(storage)),
                                           addressof(reinterpret_cast<u8 &>(expected)), reinterpret_cast<u8 &>(desired),
                                           weak, success, failure);
#endif
    }

    static T fetch_add(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch add operation.");
#if defined(QZ_COMPILER_MSVC)
        char result = _InterlockedExchangeAdd8(addressof(reinterpret_cast<volatile char &>(storage)),
                                               reinterpret_cast<char &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_add(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_sub(T &storage, T value, memory_order order) noexcept
    {
#if defined(QZ_COMPILER_MSVC)
        return fetch_add(storage, 0 - value, order);
#else
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch sub operation.");
        return __atomic_fetch_sub(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_and(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch and operation.");
#if defined(QZ_COMPILER_MSVC)
        char result =
            _InterlockedAnd8(addressof(reinterpret_cast<volatile char &>(storage)), reinterpret_cast<char &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_and(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_or(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch or operation.");
#if defined(QZ_COMPILER_MSVC)
        char result =
            _InterlockedOr8(addressof(reinterpret_cast<volatile char &>(storage)), reinterpret_cast<char &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_or(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_xor(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch xor operation.");
#if defined(QZ_COMPILER_MSVC)
        char result =
            _InterlockedXor8(addressof(reinterpret_cast<volatile char &>(storage)), reinterpret_cast<char &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_xor(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }
};

template <typename T>
struct atomic_operations<T, 2>
{
    [[nodiscard]] static T load(T &storage, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_LOAD_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        u16 value = *addressof(reinterpret_cast<volatile u16 &>(storage));
        QZ_PLACE_ATOMIC_LOAD_BARRIER(order);
#else
        u16 value = __atomic_load_n(addressof(reinterpret_cast<volatile u16 &>(storage)), order);
#endif
        return reinterpret_cast<T &>(value);
    }

    static void store(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_STORE_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        switch (order)
        {
        case memory_order_relaxed:
            *addressof(reinterpret_cast<volatile u16 &>(storage)) = reinterpret_cast<u16 &>(desired);
            break;
        case memory_order_release:
            QZ_COMPILER_BARRIER();
            *addressof(reinterpret_cast<volatile u16 &>(storage)) = reinterpret_cast<u16 &>(desired);
            break;
        case memory_order_seq_cst:
            _InterlockedExchange16(addressof(reinterpret_cast<volatile short &>(storage)),
                                   reinterpret_cast<short &>(desired));
            break;
        default:
            QZ_UNREACHABLE();
        }
#else
        __atomic_store_n(addressof(reinterpret_cast<volatile u16 &>(storage)), reinterpret_cast<u16 &>(desired), order);
#endif
    }

    static T exchange(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic exchange.");
#if defined(QZ_COMPILER_MSVC)
        short prev = _InterlockedExchange16(addressof(reinterpret_cast<volatile short &>(storage)),
                                            reinterpret_cast<short &>(desired));
#else
        u16 prev = __atomic_exchange_n(addressof(reinterpret_cast<volatile u16 &>(storage)),
                                       reinterpret_cast<u16 &>(desired), order);
#endif
        return reinterpret_cast<T &>(prev);
    }

    static bool cmpxchg(T &storage, T &expected, T desired, bool weak, memory_order success,
                        memory_order failure) noexcept
    {
        QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure);
#if defined(QZ_COMPILER_MSVC)
        QZ_UNUSED(weak);

        short required = *addressof(reinterpret_cast<volatile short &>(expected));
        short previous = _InterlockedCompareExchange16(addressof(reinterpret_cast<volatile short &>(storage)),
                                                       reinterpret_cast<short &>(desired), required);
        if (previous == required)
        {
            return true;
        }
        reinterpret_cast<short &>(expected) = previous;
        return false;
#else
        return __atomic_compare_exchange_n(addressof(reinterpret_cast<volatile u16 &>(storage)),
                                           addressof(reinterpret_cast<u16 &>(expected)),
                                           reinterpret_cast<u16 &>(desired), weak, success, failure);
#endif
    }

    static T fetch_add(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch add operation.");
#if defined(QZ_COMPILER_MSVC)
        short result = _InterlockedExchangeAdd16(addressof(reinterpret_cast<volatile short &>(storage)),
                                                 reinterpret_cast<short &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_add(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_sub(T &storage, T value, memory_order order) noexcept
    {
#if defined(QZ_COMPILER_MSVC)
        return fetch_add(storage, 0 - value, order);
#else
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch sub operation.");
        return __atomic_fetch_sub(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_and(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch and operation.");
#if defined(QZ_COMPILER_MSVC)
        short result =
            _InterlockedAnd16(addressof(reinterpret_cast<volatile short &>(storage)), reinterpret_cast<short &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_and(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_or(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch or operation.");
#if defined(QZ_COMPILER_MSVC)
        short result =
            _InterlockedOr16(addressof(reinterpret_cast<volatile short &>(storage)), reinterpret_cast<short &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_or(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_xor(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch xor operation.");
#if defined(QZ_COMPILER_MSVC)
        short result =
            _InterlockedXor16(addressof(reinterpret_cast<volatile short &>(storage)), reinterpret_cast<short &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_xor(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }
};

template <typename T>
struct atomic_operations<T, 4>
{
    [[nodiscard]] static T load(T &storage, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_LOAD_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        u32 value = *addressof(reinterpret_cast<volatile u32 &>(storage));
        QZ_PLACE_ATOMIC_LOAD_BARRIER(order);
#else
        u32 value = __atomic_load_n(addressof(reinterpret_cast<volatile u32 &>(storage)), order);
#endif
        return reinterpret_cast<T &>(value);
    }

    static void store(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_STORE_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        switch (order)
        {
        case memory_order_relaxed:
            *addressof(reinterpret_cast<volatile u32 &>(storage)) = reinterpret_cast<u32 &>(desired);
            break;
        case memory_order_release:
            QZ_COMPILER_BARRIER();
            *addressof(reinterpret_cast<volatile u32 &>(storage)) = reinterpret_cast<u32 &>(desired);
            break;
        case memory_order_seq_cst:
            _InterlockedExchange(addressof(reinterpret_cast<volatile long &>(storage)),
                                 reinterpret_cast<long &>(desired));
            break;
        default:
            QZ_UNREACHABLE();
        }
#else
        __atomic_store_n(addressof(reinterpret_cast<volatile u32 &>(storage)), reinterpret_cast<u32 &>(desired), order);
#endif
    }

    static T exchange(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic exchange.");
#if defined(QZ_COMPILER_MSVC)
        long prev = _InterlockedExchange(addressof(reinterpret_cast<volatile long &>(storage)),
                                         reinterpret_cast<long &>(desired));
#else
        u32 prev = __atomic_exchange_n(addressof(reinterpret_cast<volatile u32 &>(storage)),
                                       reinterpret_cast<u32 &>(desired), order);
#endif
        return reinterpret_cast<T &>(prev);
    }

    static bool cmpxchg(T &storage, T &expected, T desired, bool weak, memory_order success,
                        memory_order failure) noexcept
    {
        QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure);
#if defined(QZ_COMPILER_MSVC)
        QZ_UNUSED(weak);

        long required = *addressof(reinterpret_cast<volatile long &>(expected));
        long previous = _InterlockedCompareExchange(addressof(reinterpret_cast<volatile long &>(storage)),
                                                    reinterpret_cast<long &>(desired), required);

        if (previous == required)
        {
            return true;
        }

        reinterpret_cast<long &>(expected) = previous;
        return false;
#else
        return __atomic_compare_exchange_n(addressof(reinterpret_cast<volatile u32 &>(storage)),
                                           addressof(reinterpret_cast<u32 &>(expected)),
                                           reinterpret_cast<u32 &>(desired), weak, success, failure);
#endif
    }

    static T fetch_add(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch add operation.");
#if defined(QZ_COMPILER_MSVC)
        long result = _InterlockedExchangeAdd(addressof(reinterpret_cast<volatile long &>(storage)),
                                              reinterpret_cast<long &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_add(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_sub(T &storage, T value, memory_order order) noexcept
    {
#if defined(QZ_COMPILER_MSVC)
        return fetch_add(storage, 0 - value, order);
#else
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch sub operation.");
        return __atomic_fetch_sub(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_and(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch and operation.");
#if defined(QZ_COMPILER_MSVC)
        long result =
            _InterlockedAnd(addressof(reinterpret_cast<volatile long &>(storage)), reinterpret_cast<long &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_and(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_or(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch or operation.");
#if defined(QZ_COMPILER_MSVC)
        long result =
            _InterlockedOr(addressof(reinterpret_cast<volatile long &>(storage)), reinterpret_cast<long &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_or(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_xor(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch xor operation.");
#if defined(QZ_COMPILER_MSVC)
        long result =
            _InterlockedXor(addressof(reinterpret_cast<volatile long &>(storage)), reinterpret_cast<long &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_xor(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }
};

template <typename T>
struct atomic_operations<T, 8>
{
    [[nodiscard]] static T load(T &storage, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_LOAD_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        u64 value = *addressof(reinterpret_cast<volatile u64 &>(storage));
        QZ_PLACE_ATOMIC_LOAD_BARRIER(order);
#else
        u64 value = __atomic_load_n(addressof(reinterpret_cast<volatile u64 &>(storage)), order);
#endif
        return reinterpret_cast<T &>(value);
    }

    static void store(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_STORE_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        switch (order)
        {
        case memory_order_relaxed:
            *addressof(reinterpret_cast<volatile u64 &>(storage)) = reinterpret_cast<u64 &>(desired);
            break;
        case memory_order_release:
            QZ_COMPILER_BARRIER();
            *addressof(reinterpret_cast<volatile u64 &>(storage)) = reinterpret_cast<u64 &>(desired);
            break;
        case memory_order_seq_cst:
            _InterlockedExchange64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                   reinterpret_cast<__int64 &>(desired));
            break;
        default:
            QZ_UNREACHABLE();
        }
#else
        __atomic_store_n(addressof(reinterpret_cast<volatile u64 &>(storage)), reinterpret_cast<u64 &>(desired), order);
#endif
    }

    static T exchange(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic exchange.");
#if defined(QZ_COMPILER_MSVC)
        __int64 prev = _InterlockedExchange64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                              reinterpret_cast<__int64 &>(desired));
#else
        u64 prev = __atomic_exchange_n(addressof(reinterpret_cast<volatile u64 &>(storage)),
                                       reinterpret_cast<u64 &>(desired), order);
#endif
        return reinterpret_cast<T &>(prev);
    }

    static bool cmpxchg(T &storage, T &expected, T desired, bool weak, memory_order success,
                        memory_order failure) noexcept
    {
        QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure);
#if defined(QZ_COMPILER_MSVC)
        QZ_UNUSED(weak); // msvc STL only uses strong cmpxchg.

        __int64 required = *addressof(reinterpret_cast<volatile __int64 &>(expected));
        __int64 previous = _InterlockedCompareExchange64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                                         reinterpret_cast<__int64 &>(desired), required);
        if (previous == required)
        {
            return true;
        }
        reinterpret_cast<__int64 &>(expected) = previous;
        return false;
#else
        return __atomic_compare_exchange_n(addressof(reinterpret_cast<volatile u64 &>(storage)),
                                           addressof(reinterpret_cast<u64 &>(expected)),
                                           reinterpret_cast<u64 &>(desired), weak, success, failure);
#endif
    }

    static T fetch_add(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch add operation.");
#if defined(QZ_COMPILER_MSVC)
        __int64 result = _InterlockedExchangeAdd64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                                   reinterpret_cast<__int64 &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_add(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_sub(T &storage, T value, memory_order order) noexcept
    {
#if defined(QZ_COMPILER_MSVC)
        return fetch_add(storage, 0 - value, order);
#else
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch sub operation.");
        return __atomic_fetch_sub(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_and(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch and operation.");
#if defined(QZ_COMPILER_MSVC)
        __int64 result = _InterlockedAnd64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                           reinterpret_cast<__int64 &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_and(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_or(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch or operation.");
#if defined(QZ_COMPILER_MSVC)
        __int64 result = _InterlockedOr64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                          reinterpret_cast<__int64 &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_or(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }

    static T fetch_xor(T &storage, T value, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic fetch xor operation.");
#if defined(QZ_COMPILER_MSVC)
        __int64 result = _InterlockedXor64(addressof(reinterpret_cast<volatile __int64 &>(storage)),
                                           reinterpret_cast<__int64 &>(value));
        return reinterpret_cast<T &>(result);
#else
        return __atomic_fetch_xor(addressof(reinterpret_cast<volatile T &>(storage)), value, order);
#endif
    }
};

template <typename T>
struct atomic_operations<T, 16>
{
    [[nodiscard]] static T load(T &storage, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_LOAD_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        alignas(16) __int64 value[2] = {0, 0};
        (void)_InterlockedCompareExchange128(static_cast<volatile __int64 *>(addressof(storage)), 0, 0, value);
        return *reinterpret_cast<T *>(value);
#elif defined(QZ_COMPILER_CLANG)
        alignas(16) __int128 value;
        __asm__ __volatile__("movdqa %1, %0" : "=x"(value) : "m"(storage));
        return reinterpret_cast<T &>(value);
#else
        __int128 value = __atomic_load_n(addressof(reinterpret_cast<volatile __int128 &>(storage)), order);
        return reinterpret_cast<T &>(value);
#endif
    }

    static void store(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_ATOMIC_STORE_ORDER(order);
#if defined(QZ_COMPILER_MSVC)
        (void)exchange(storage, desired, order);
#elif defined(QZ_COMPILER_CLANG)
        switch (order)
        {
        case memory_order_relaxed:
            __asm__ __volatile__("movdqa %1, %0" : "=x"(storage) : "m"(desired));
            break;
        case memory_order_release:
            QZ_COMPILER_BARRIER();
            __asm__ __volatile__("movdqa %1, %0" : "=x"(storage) : "m"(desired));
            break;
        case memory_order_seq_cst:
            (void)exchange(storage, desired, memory_order_seq_cst);
            break;
        default:
            QZ_UNREACHABLE();
        }
#else
        __atomic_store_n(addressof(reinterpret_cast<volatile __int128 &>(storage)),
                         reinterpret_cast<__int128 &>(desired), order);
#endif
    }

    static T exchange(T &storage, T desired, memory_order order) noexcept
    {
        QZ_VERIFY_MSG(order <= memory_order_seq_cst, "Invalid memory order constraint for atomic exchange.");
#if defined(QZ_COMPILER_MSVC) || defined(QZ_COMPILER_CLANG)
        T previous = desired;
        while (!cmpxchg(storage, previous, desired, false, memory_order_seq_cst, memory_order_seq_cst))
        {
            // keep trying until previous value and storage are equal
        }
        return previous;
#else
        __int128 prev = __atomic_exchange_n(addressof(reinterpret_cast<volatile __int128 &>(storage)),
                                            reinterpret_cast<__int128 &>(desired), order);
        return reinterpret_cast<T &>(prev);
#endif
    }

    static bool cmpxchg(T &storage, T &expected, T desired, bool weak, memory_order success,
                        memory_order failure) noexcept
    {
        QZ_VERIFY_ATOMIC_CMPXCHG_ORDER(success, failure);
#if defined(QZ_COMPILER_MSVC)
        QZ_UNUSED(weak);

        alignas(16) __int64 storageData[2] = {
            reinterpret_cast<__int64 *>(addressof(storage))[0],
            reinterpret_cast<__int64 *>(addressof(storage))[1],
        };

        auto result = _InterlockedCompareExchange128(
            reinterpret_cast<volatile __int64 *>(addressof(storage)), // - storage as i64 pointer
            reinterpret_cast<__int64 *>(addressof(desired))[1],       // - desired high
            reinterpret_cast<__int64 *>(addressof(desired))[0],       // - desired low
            reinterpret_cast<__int64 *>(addressof(expected))          // - expected data
        );

        if (result)
        {
            return true;
        }

        reinterpret_cast<__int64 *>(addressof(expected))[0] = storageData[0];
        reinterpret_cast<__int64 *>(addressof(expected))[1] = storageData[1];
        return false;
#elif defined(QZ_COMPILER_CLANG)
        QZ_UNUSED(weak);

        bool result;
        QZ_COMPILER_BARRIER();
        // See: https://www.felixcloutier.com/x86/cmpxchg8b:cmpxchg16b
        __asm__ __volatile__(
            "lock cmpxchg16b %1"
            : "=@ccz"(result), "+m"(storage),
              "+a"(reinterpret_cast<s64 *>(addressof(expected))[0]), // read/write register RAX (lower half)
              "+d"(reinterpret_cast<s64 *>(addressof(expected))[1])  // read/write register RDX (upper half)
            : "b"(reinterpret_cast<s64 *>(addressof(desired))[0]),   // read register RBX (lower half)
              "c"(reinterpret_cast<s64 *>(addressof(desired))[1])    // read register RCX (upper half)
            : "memory", "cc");
        QZ_COMPILER_BARRIER();

        return result;
#else
        return __atomic_compare_exchange_n(addressof(reinterpret_cast<volatile __int128 &>(storage)),
                                           addressof(reinterpret_cast<__int128 &>(expected)),
                                           reinterpret_cast<u8 &>(desired), weak, success, failure);
#endif
    }
};

///
/// @brief Check whether all the atomic operations of a given type are lock free or not.
/// @tparam T The type of value.
/// @return True if the atomic operations are always lock free, else false.
///
template <typename T>
[[nodiscard]] constexpr bool atomic_operations_are_always_lock_free()
{
#if defined(QZ_COMPILER_MSVC)
    return sizeof(T) <= 8 && ((sizeof(T) - 1) & sizeof(T)) == 0;
#else
    return __atomic_always_lock_free(sizeof(T), nullptr);
#endif
}

///
/// @brief Check whether the atomic operations of a given type are lock free or not.
/// @tparam T The type of value.
/// @return True if the atomic operations are lock free, else false.
///
template <typename T>
[[nodiscard]] bool atomic_operations_are_lock_free()
{
#if defined(QZ_COMPILER_MSVC)
    return atomic_operations_are_always_lock_free<T>();
#else
    return __atomic_is_lock_free(sizeof(T), nullptr);
#endif
}

///
/// @brief Ensures synchronization of operations according to the `memory_order` constraints.
/// @details Implementation detail:
/// - Does nothing if `order == memory_order_relaxed`.
/// - Generates compiler barriers and a CPU barrier instruction if `order == memory_order_seq_cst`.
/// - Generates a compiler barrier for any other constraints.
///
inline void atomic_thread_fence(memory_order order) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
    if (order == memory_order_relaxed)
    {
        return;
    }
    QZ_COMPILER_BARRIER();
    if (order == memory_order_seq_cst)
    {
        __faststorefence(); // generates a dummy interlocked or operation
        QZ_COMPILER_BARRIER();
    }
#else
    __atomic_thread_fence(order);
#endif
}

///
/// @brief Ensures synchronization of operations according to the `memory_order` constraints without generating a CPU
/// instruction for it.
/// @details Implementation detail:
/// - Does nothing if `order == memory_order_relaxed`.
/// - Generates a compiler barrier for any other constraints.
///
inline void atomic_signal_fence(memory_order order)
{
#if defined(_MSC_VER) && !defined(__clang__)
    if (order != memory_order_relaxed)
    {
        QZ_COMPILER_BARRIER();
    }
#else
    __atomic_signal_fence(order);
#endif
}

} // namespace qz

// these macros are no longer needed.
#undef QZ_VERIFY_ATOMIC_LOAD_ORDER
#undef QZ_VERIFY_ATOMIC_STORE_ORDER
#undef QZ_VERIFY_ATOMIC_CMPXCHG_ORDER
#undef QZ_PLACE_ATOMIC_LOAD_BARRIER
#undef QZ_COMPILER_BARRIER
