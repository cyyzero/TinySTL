#ifndef POINTER_TRAITS_H
#define POINTER_TRAITS_H

#include <type_traits>

namespace cyy
{
template<typename Ptr>
struct pointer_traits
{
private:
    // test whether Ptr::element_type exits
    struct element_type_tester
    {
        template<typename T>
        static std::true_type test(typename T::element_type*);

        template<typename>
        static std::false_type test(...);

        using type = decltype(test<Ptr>(nullptr));
    };

    // if Ptr::element_type exits, type is that
    template<typename P, bool = element_type_tester::type::value>
    struct element_type_helper;

    template<typename P>
    struct element_type_helper<P, true>
    {
        using type = typename P::element_type;
    };

    // else if Ptr's type is tempalte<T, Args...>, type = T;
    template<template<typename, typename...> typename P, typename T, typename... Args>
    struct element_type_helper<P<T, Args...>, false>
    {
        using type = T;
    };

    // test whether ptr::difference_type exits
    struct difference_type_helper
    {
        template<typename T>
        static typename T::difference_type test(int);

        template<typename>
        static std::ptrdiff_t test(...);

        using type = decltype(test<Ptr>(0));
    };

    // test whether Ptr::rebind<U> exits
    template<typename U>
    struct rebind_tester
    {
        template<typename T>
        static std::true_type test(typename T::template rebind<U>*);

        template<typename>
        static std::false_type test(...);

        using type = decltype(test<Ptr>(nullptr));
    };

    template<typename P, typename U, bool = rebind_tester<U>::type::value>
    struct rebind_helper;

    template<typename P, typename U>
    struct rebind_helper<P, U, true>
    {
        using type = typename P::template rebind<U>;
    };

    template<template<typename, typename...> typename P, typename U, typename T, typename... Args>
    struct rebind_helper<P<T, Args...>, U, false>
    {
        using type = P<U, Args...>;
    };

public:
    using pointer         = Ptr;

    // Ptr::element_type if present. Otherwise T if Ptr is a template instantiation Template<T, Args...>
    using element_type    = typename element_type_helper<Ptr>::type;

    // Ptr::difference_type if present, otherwise std::ptrdiff_t
    using difference_type = typename difference_type_helper::type;

    // Ptr::rebind<U> if exists, otherwise Template<U, Args...>
    // if Ptr is a template instantiation Template<T, Args...>
    template<typename U>
    using rebind = typename rebind_helper<Ptr, U>::type;


    static pointer pointer_to(element_type& r)
    {
        // if Ptr does not provide a static member function pointer_to, instantiation of this function is a compile-time error.
        return Ptr::pointer_to(r);
    }
};

template<typename T>
struct pointer_traits<T*>
{
    using pointer      = T*;
    using element_type = T;
    using difference_type = std::ptrdiff_t; 

    template<typename U>
    using rebind = U*;

    static pointer pointer_to(element_type& r) noexcept
    {
        return std::addressof(r);
    }
};
} // namespace cyy
#endif // !POINTER_TRAITS_H