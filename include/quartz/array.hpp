#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>

#include "quartz/types.hpp"
#include "quartz/utilities.hpp"

///
/// @defgroup QzContainers ADT Containers
/// @brief A collection of abstract data type containers.
///

namespace qz
{

///
/// @ingroup QzContainers
///
/// @brief A wrapper over a C-style fixed array.
///
/// @tparam T The array element type.
/// @tparam N The number of elements in the array.
///
template <class T, size_t N>
struct array
{
    // TYPEDEFS

    using value_type             = T;
    using size_type              = decltype(N);
    using difference_type        = ssz;
    using reference              = value_type &;
    using const_reference        = const value_type &;
    using pointer                = value_type *;
    using const_pointer          = const value_type *;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // METHODS

    /// @brief Fill the contents of this array with the given value.
    /// @param value The value being filled into the array.
    constexpr void fill(const value_type &value)
    {
        for (size_type i = 0; i < N; ++i)
        {
            m_data[i] = value;
        }
    }

    /// @brief Swap the contents of this array with those of the other array.
    /// @param other The other array container to swap with.
    constexpr void swap(array &other) noexcept(std::is_nothrow_swappable_v<T>)
    {
        qz::swap(m_data, other.m_data);
    }

    /// @brief Get a reference to the element at the given position.
    /// @param pos The position of the element. If out of bounds, throws an exception.
    [[nodiscard]] constexpr reference at(size_type pos)
    {
        if (pos >= N)
        {
            throw std::out_of_range("Index out of bounds access.");
        }
        return m_data[pos];
    }

    /// @brief Get a const reference to the element at the given position.
    /// @param pos The position of the element. If out of bounds, throws an exception.
    [[nodiscard]] constexpr const_reference at(size_type pos) const
    {
        if (pos >= N)
        {
            throw std::out_of_range("Index out of bounds access.");
        }
        return m_data[pos];
    }

    /// @brief Get a reference to the first element.
    [[nodiscard]] constexpr reference front()
    {
        return m_data[0];
    }

    /// @brief Get a const reference to the first element.
    [[nodiscard]] constexpr const_reference front() const
    {
        return m_data[0];
    }

    /// @brief Get a reference to the last element.
    [[nodiscard]] constexpr reference back()
    {
        return m_data[N - 1];
    }

    /// @brief Get a const reference to the last element.
    [[nodiscard]] constexpr const_reference back() const
    {
        return m_data[N - 1];
    }

    /// @brief Get a pointer to the raw data.
    [[nodiscard]] constexpr pointer data()
    {
        return m_data;
    }

    /// @brief Get a const pointer to the raw data.
    [[nodiscard]] constexpr const_pointer data() const
    {
        return m_data;
    }

    /// @brief True if the size of this array is zero, else false.
    [[nodiscard]] constexpr bool empty() const
    {
        return false;
    }

    /// @brief Get the size of this array (i.e. number of elements in this array).
    [[nodiscard]] constexpr size_type size() const
    {
        return N;
    }

    /// @brief Get the maximum number of elements that may fit into this array. Always equal to size().
    [[nodiscard]] constexpr size_type max_size() const
    {
        return size();
    }

    /// @brief Get an iterator to the beginning of this array.
    [[nodiscard]] constexpr iterator begin()
    {
        return data();
    }

    /// @brief Get a const iterator to the beginning of this array.
    [[nodiscard]] constexpr const_iterator begin() const
    {
        return cbegin();
    }

    /// @brief Get a const iterator to the beginning of this array.
    [[nodiscard]] constexpr const_iterator cbegin() const
    {
        return data();
    }

    /// @brief Get an iterator past the last element of this array.
    [[nodiscard]] constexpr iterator end()
    {
        return data() + N;
    }

    /// @brief Get a const iterator past the last element of this array.
    [[nodiscard]] constexpr const_iterator end() const
    {
        return cend();
    }

    /// @brief Get a const iterator past the last element of this array.
    [[nodiscard]] constexpr const_iterator cend() const
    {
        return data() + N;
    }

    /// @brief Get a reverse iterator to the last element of this array.
    [[nodiscard]] constexpr reverse_iterator rbegin()
    {
        return std::make_reverse_iterator(end());
    }

    /// @brief Get a reverse const iterator to the last element of this array.
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    /// @brief Get a reverse const iterator to the last element of this array.
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const
    {
        return std::make_reverse_iterator(cend());
    }

    /// @brief Get a reverse iterator past the first element of this array.
    [[nodiscard]] constexpr reverse_iterator rend()
    {
        return std::make_reverse_iterator(begin());
    }

    /// @brief Get a reverse const iterator past the first element of this array.
    [[nodiscard]] constexpr const_reverse_iterator rend() const
    {
        return crend();
    }

    /// @brief Get a reverse const iterator past the first element of this array.
    [[nodiscard]] constexpr const_reverse_iterator crend() const
    {
        return std::make_reverse_iterator(cbegin());
    }

    // OPERATOR OVERLOADS

    /// @brief Get a reference to the element at the given position.
    /// @param pos The position of the element. No bounds checking is done.
    [[nodiscard]] constexpr reference operator[](size_type pos)
    {
        return m_data[pos];
    }

    /// @brief Get a const reference to the element at the given position.
    /// @param pos The position of the element. No bounds checking is done.
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const
    {
        return m_data[pos];
    }

    // DATA MEMBERS

    /// @brief The underlying array. Made public to allow aggregate initialization.
    T m_data[N]; // NOLINT
};

///
/// @brief Template specialization of qz::array<T,N> for zero length arrays. There are no data members, and calls to
/// data() returns nullptr instead.
///
template <class T>
struct array<T, 0>
{
    // TYPEDEFS

    using value_type             = T;
    using size_type              = usz;
    using difference_type        = ssz;
    using reference              = value_type &;
    using const_reference        = const value_type &;
    using pointer                = value_type *;
    using const_pointer          = const value_type *;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // METHODS

    constexpr void fill([[maybe_unused]] const value_type &value)
    {
        // empty.
    }

    constexpr void swap([[maybe_unused]] const array &other) noexcept
    {
        // empty.
    }

    [[nodiscard]] constexpr pointer data()
    {
        return nullptr;
    }

    [[nodiscard]] constexpr const_pointer data() const
    {
        return nullptr;
    }

    [[nodiscard]] constexpr reference at([[maybe_unused]] size_type pos)
    {
        throw std::out_of_range("Index out of bounds access.");
    }

    [[nodiscard]] constexpr const_reference at([[maybe_unused]] size_type pos) const
    {
        throw std::out_of_range("Index out of bounds access.");
    }

    [[nodiscard]] constexpr reference front()
    {
        return *data();
    }

    [[nodiscard]] constexpr const_reference front() const
    {
        return *data();
    }

    [[nodiscard]] constexpr reference back()
    {
        return *data();
    }

    [[nodiscard]] constexpr const_reference back() const
    {
        return *data();
    }

    [[nodiscard]] constexpr bool empty() const
    {
        return true;
    }

    [[nodiscard]] constexpr size_type size() const
    {
        return 0;
    }

    [[nodiscard]] constexpr size_type max_size() const
    {
        return size();
    }

    [[nodiscard]] constexpr iterator begin()
    {
        return data();
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
        return cbegin();
    }

    [[nodiscard]] constexpr const_iterator cbegin() const
    {
        return data();
    }

    [[nodiscard]] constexpr iterator end()
    {
        return data();
    }

    [[nodiscard]] constexpr const_iterator end() const
    {
        return cend();
    }

    [[nodiscard]] constexpr const_iterator cend() const
    {
        return data();
    }

    [[nodiscard]] constexpr reverse_iterator rbegin()
    {
        return std::make_reverse_iterator(end());
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const
    {
        return std::make_reverse_iterator(cend());
    }

    [[nodiscard]] constexpr reverse_iterator rend()
    {
        return std::make_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const
    {
        return crend();
    }

    [[nodiscard]] constexpr const_reverse_iterator crend() const
    {
        return std::make_reverse_iterator(cbegin());
    }

    //

    [[nodiscard]] constexpr reference operator[]([[maybe_unused]] size_type pos)
    {
        return *data();
    }

    [[nodiscard]] constexpr const_reference operator[]([[maybe_unused]] size_type pos) const
    {
        return *data();
    }
};

//

///
/// @ingroup QzUtilities
///
/// @brief Swap the contents of two arrays.
/// @tparam T The value type of the array.
/// @tparam N The size of the array.
/// @param arrayA The first array.
/// @param arrayB The second array.
template <class T, usz N>
constexpr void swap(array<T, N> &arrayA, array<T, N> &arrayB) noexcept(std::is_nothrow_swappable_v<T>)
{
    arrayA.swap(arrayB);
}

/// @brief Type deduction guide for qz::array
template <class T, class... U>
array(T, U...) -> array<T, 1 + sizeof...(U)>;

} // namespace qz
