#ifndef _ARRAY_H
#define _ARRAY_H

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <utility>

namespace cyyzero
{

    template<typename T, std::size_t N>
    struct __array_traits
    {
        using _type = T[N];

        static constexpr T& _S_ref(const _type& t, std::size_t pos) noexcept
        {
            return const_cast<T&>(t[pos]);
        }

        static constexpr T* _S_ptr(const _type& t) noexcept
        {
            return const_cast<T*>(t);
        }
    };

    template<typename T>
    struct __array_traits<T, 0>
    {
        struct _type { };

        static constexpr T& _S_ref(const _type&, std::size_t) noexcept
        {
            return *static_cast<T*>(nullptr);
        }

        static constexpr T* _S_ptr(const _type&) noexcept
        {
            return nullptr;
        }
    };

    template<typename T, std::size_t N>
    struct array
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

        
        using _array_type = __array_traits<T, N>;
        typename _array_type::_type _M_elems;

        // Element access
        constexpr reference at(size_type pos)
        {
            if (pos >= N)
                throw std::out_of_range("pos too large");
            return _array_type::_S_ref(_M_elems, pos);
        }

        constexpr const_reference at(size_type pos) const
        {
            if (pos >= N)
                throw std::out_of_range("pos too large");
            return _array_type::_S_ref(_M_elems, pos);
        }

        constexpr reference operator[](size_type pos) noexcept
        {
            return _array_type::_S_ref(_M_elems, pos);
        }

        constexpr const_reference operator[](size_type pos) const noexcept
        {
            return _array_type::_S_ref(_M_elems, pos);
        }

        constexpr reference front()
        {
            return _array_type::_S_ref(_M_elems, 0);
        }

        constexpr const_reference front() const
        {
            return _array_type::_S_ref(_M_elems, 0);
        }

        constexpr reference back()
        {
            return N ? _array_type::_S_ref(_M_elems, N-1)
                     : _array_type::_S_ref(_M_elems, 0);
        }

        constexpr const_reference back() const
        {
            return N ? _array_type::_S_ref(_M_elems, N-1)
                     : _array_type::_S_ref(_M_elems, 0);
        }

        constexpr pointer data() noexcept
        {
            return _array_type::_S_ptr(_M_elems);
        }

        constexpr const_pointer data() const noexcept
        {
            return _array_type::_S_ptr(_M_elems);
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

        void swap(array& other) noexcept(noexcept(swap(std::declval<T&>(), std::declval<T&>())))
        {
            std::swap_ranges(begin(), end(), other.begin());
        }
    };

    template<typename T, std::size_t N>
    inline bool operator==(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename T, std::size_t N>
    inline bool operator!=(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename T, std::size_t N>
    inline bool operator<(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename T, std::size_t N>
    inline bool operator>(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return rhs < lhs;
    }

    template<typename T, std::size_t N>
    inline bool operator<=(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return !(lhs > rhs);
    }

    template<typename T, std::size_t N>
    inline bool operator>=(const array<T, N>& lhs, const array<T, N>& rhs)
    {
        return !(lhs < rhs);
    }

    template<std::size_t I, typename T, std::size_t N>
    constexpr T& get(array<T, N>& a) noexcept
    {
        static_assert(I < N, "index is out of bound");
        return a.at(I);
    }

    template<std::size_t I, typename T, std::size_t N>
    constexpr T&& get(array<T, N>&& a) noexcept
    {
        return std::move(get<I>(a));
    }

    template<std::size_t I, typename T, std::size_t N>
    constexpr const T& get(const array<T, N>& a) noexcept
    {
        return static_cast<const T&>(
            get<I>(const_cast<array<T, N>&>(a))
        );
    }

    template<std::size_t I, typename T, std::size_t N>
    constexpr const T&& get(const array<T, N>&& a) noexcept
    {
        return std::remove(get<I>(a));
    }

}
#endif