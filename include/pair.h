#ifndef PAIR_H
#define PAIR_H

#include <type_traits>
#include <tuple>

namespace cyyzero
{

struct piecewise_construct_t { };

inline constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();

template<int...>
struct _Index_tuple;

template <typename T1, typename T2>
struct pair
{
    using first_type  = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    constexpr pair()
        : first(), second() { }
    
    constexpr pair(const T1& x, const T2& y)
        : first(x), second(y) { }

    constexpr pair(const pair&) = default;

    constexpr pair(pair&&) = default;

    template<typename U1, typename = typename
             std::enable_if<std::is_convertible<U1, T1>::value>::type>
    constexpr pair(U1&& x, const T2& y)
        : first(std::forward<U1>(x)), second(y) { }

    template<typename U2, typename = typename
             std::enable_if<std::is_convertible<U2, T2>::value>::type>
    constexpr pair(const T1& x, U2&& y)
        : first(x), second(std::forward<U2>(y)) { }

    template<typename U1, typename U2, typename = typename 
             std::enable_if<std::conjunction<std::is_convertible<U1, T1>,
                                             std::is_convertible<U2, T2>>::value>::type>
    constexpr pair(U1&& x, U2&& y)
        : first(std::forward<U1>(x)), second(std::forward<U2>(y)) { }

    template<typename U1, typename U2, typename = typename
            std::enable_if<std::conjunction<std::is_convertible<T1, U1>,
                                            std::is_convertible<T2, U2>>::value>::type>
    constexpr pair(const pair<U1, U2>& p)
        : first(p.first), second(p.second) { }

    template<typename U1, typename U2, typename = typename
             std::enable_if<std::conjunction<std::is_convertible<T1, U1>,
                                             std::is_convertible<T2, U2>>::value>::type>
    constexpr pair(pair<U1, U2>&& p)
        : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) { }

    template<typename... Args1, typename... Args2>
    pair(piecewise_construct_t, std::tuple<Args1...> first_args, std::tuple<Args2...> second_args)
        : first(), second() { }

    pair& operator=(const pair &other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    pair& operator=(pair &&p)
    {
        first = std::move(p.first);
        second = std::move(p.second);
        return *this;
    }

    template<typename U1, typename U2>
    pair& operator=(const pair<U1, U2> &p)
    {
        first = p.first;
        second = p.second;
        return *this;
    }

    template<typename U1, typename U2>
    pair& operator=(pair<U1, U2> &&p)
    {
        first = std::move(p.first);
        second = std::move(p.second);
        return *this;
    }

    void swap(pair &p)
    {
        using std::swap;
        swap(first, p.first);
        swap(second, p.second);
    }

private:
    template<typename Tp, typename... Args>
    static Tp _cons(std::tuple<Args...>&&)
    {
        // TODO
    }

    template<typename Tp, typename... Args, int... Indexes>
    static Tp _do_cons(std::tuple<Args...>&&, const _Index_tuple<Indexes...>&)
    {
        // TODO
    }
}; // struct pair

template<typename T1, typename T2>
inline constexpr bool operator==(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename T1, typename T2>
inline constexpr bool operator!=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs)
{
    return !(lhs == rhs);
}

template<typename T1, typename T2>
inline bool operator<(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs)
{
    if (lhs.first < rhs.first)
        return true;
    if (rhs.first < lhs.first)
        return false;
    if (lhs.second < rhs.second)
        return true;

    return false;
}

template< typename T1, typename T2 >
inline constexpr bool operator<=(const pair<T1,T2> &lhs, const pair<T1,T2> &rhs)
{
    return !(rhs < lhs);
}

template< typename T1, typename T2 >
inline constexpr bool operator>(const pair<T1,T2> &lhs, const pair<T1,T2> &rhs)
{
    return rhs < lhs;
}

template< typename T1, typename T2 >
inline constexpr bool operator>=(const pair<T1,T2> &lhs, const pair<T1,T2> &rhs)
{
    return !(lhs < rhs);
}

template<typename T1, typename T2>
inline void swap(pair<T1, T2> &lhs, pair<T1, T2> &rhs)
{
    lhs.swap(rhs);
}

namespace
{
template<typename T>
struct strip_reference_wrapper
{
    using type = T;
};

template<typename T>
struct strip_reference_wrapper<std::reference_wrapper<T>>
{
    using type = T&;
};

template<typename T>
struct decay_and_strip
{
    using type = typename strip_reference_wrapper<typename std::decay<T>::type>::type;
};

} // unnamed namespace


template<typename T1, typename T2>
inline pair<typename decay_and_strip<T1>::type, typename decay_and_strip<T2>::type>
make_pair(T1 &&first, T2 &&second)
{
    return pair<typename decay_and_strip<T1>::type, 
                    typename decay_and_strip<T2>::type>
           (std::forward<T1>(first), std::forward<T2>(second));
}

// TODO: get<>, class tuple_size<pair>, class tuple_element<pair>

} // namespace cyyzero

#endif // PAIR_H