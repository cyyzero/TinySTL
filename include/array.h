#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <utility>

namespace cyy
{
namespace detail
{
template<typename T, std::size_t N>
struct array_traits
{
    using type = T[N];

    static constexpr T& ref(const type& t, std::size_t pos) noexcept
    {
        return const_cast<T&>(t[pos]);
    }

    static constexpr T* ptr(const type& t) noexcept
    {
        return const_cast<T*>(t);
    }
};

template<typename T>
struct array_traits<T, 0>
{
    struct type { };

    static constexpr T& ref(const type&, std::size_t) noexcept
    {
        return *static_cast<T*>(nullptr);
    }

    static constexpr T* ptr(const type&) noexcept
    {
        return nullptr;
    }
};
} // namespace detail

template<typename T, std::size_t N>
struct Array
{
    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    using array_type = detail::array_traits<T, N>;
    typename array_type::type elems;

    // Element access
    constexpr reference at(size_type pos)
    {
        if (pos >= N)
            throw std::out_of_range("pos too large");
        return array_type::ref(elems, pos);
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= N)
            throw std::out_of_range("pos too large");
        return array_type::ref(elems, pos);
    }

    constexpr reference operator[](size_type pos) noexcept
    {
        return array_type::ref(elems, pos);
    }

    constexpr const_reference operator[](size_type pos) const noexcept
    {
        return array_type::ref(elems, pos);
    }

    constexpr reference front()
    {
        return array_type::ref(elems, 0);
    }

    constexpr const_reference front() const
    {
        return array_type::ref(elems, 0);
    }

    constexpr reference back()
    {
        return N ? array_type::ref(elems, N-1) : array_type::ref(elems, 0);
    }

    constexpr const_reference back() const
    {
        return N ? array_type::ref(elems, N-1) : array_type::S_ref(elems, 0);
    }

    constexpr pointer data() noexcept
    {
        return array_type::ptr(elems);
    }

    constexpr const_pointer data() const noexcept
    {
        return array_type::ptr(elems);
    }

    // Iterate

    constexpr iterator begin() noexcept
    {
        return static_cast<iterator>(data());
    }

    constexpr const_iterator begin() const noexcept
    {
        return static_cast<const_iterator>(data());
    }

    constexpr const_iterator cbegin() noexcept
    {
        return static_cast<const_iterator>(data());
    }

    constexpr iterator end() noexcept
    {
        return static_cast<iterator>(data() + N);
    }

    constexpr const_iterator end() const noexcept
    {
        return static_cast<const_iterator>(data() + N);
    }

    constexpr const_iterator cend() const noexcept
    {
        return static_cast<const_iterator>(data() + N);
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    // Capacity

    constexpr bool empty() const noexcept
    {
        return begin() == end();
    }

    size_type size() const noexcept
    {
        return N;
    }

    constexpr size_type max_size() const noexcept
    {
        return N;
    }

    // Operation
    void fill(const T& value)
    {
        std::fill_n(begin(), N, value);
    }

    void swap(Array& other) noexcept(noexcept(swap(std::declval<T&>(), std::declval<T&>())))
    {
        std::swap_ranges(begin(), end(), other.begin());
    }
};

template<typename T, std::size_t N>
inline bool operator==(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, std::size_t N>
inline bool operator!=(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return !(lhs == rhs);
}

template<typename T, std::size_t N>
inline bool operator<(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, std::size_t N>
inline bool operator>(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return rhs < lhs;
}

template<typename T, std::size_t N>
inline bool operator<=(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return !(lhs > rhs);
}

template<typename T, std::size_t N>
inline bool operator>=(const Array<T, N>& lhs, const Array<T, N>& rhs)
{
    return !(lhs < rhs);
}

template<std::size_t I, typename T, std::size_t N>
constexpr T& get(Array<T, N>& a) noexcept
{
    static_assert(I < N, "index is out of bound");
    return a.at(I);
}

template<std::size_t I, typename T, std::size_t N>
constexpr T&& get(Array<T, N>&& a) noexcept
{
    return std::move(get<I>(a));
}

template<std::size_t I, typename T, std::size_t N>
constexpr const T& get(const Array<T, N>& a) noexcept
{
    return static_cast<const T&>(
        get<I>(const_cast<Array<T, N>&>(a))
    );
}

template<std::size_t I, typename T, std::size_t N>
constexpr const T&& get(const Array<T, N>&& a) noexcept
{
    return std::remove(get<I>(a));
}

}
#endif