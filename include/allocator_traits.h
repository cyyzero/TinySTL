#ifndef ALLOCATOR_TRAITS_H
#define ALLOCATOR_TRAITS_H

#include <type_traits>
#include <utility>
#include <limits>
#include "pointer_traits.h"

namespace cyy
{
template<typename Alloc, typename T>
struct rebind_alloc_tester
{
    template<typename Alloc2, typename T2>
    static constexpr
    std::true_type test(typename Alloc2::template rebind<T2>::other*);

    template<typename, typename>
    static constexpr
    std::false_type test(...);

    using type = decltype(test<Alloc, T>(nullptr));
};

template<typename Alloc, typename T, bool = rebind_alloc_tester<Alloc, T>::type::value>
struct rebind_alloc_helper;

template<typename Alloc, typename T>
struct rebind_alloc_helper<Alloc, T, true>
{
    using type = typename Alloc::template rebind<T>::other;
};

template<template<typename, typename...>typename Alloc, typename T, typename U, typename... Args>
struct rebind_alloc_helper<Alloc<U, Args...>, T, false>
{
    using type = Alloc<T, Args...>;
};

template<typename Alloc>
struct Allocator_traits
{
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;

// helper macro using SFINAE, define T1 as Alloc::T1 if present, otherwise T2
#define ALIAS_USING_SFINAE(T1, T2)                           \
private:                                                     \
    template<typename T>                                     \
    static typename T::T1 T1##test(T*);                      \
                                                             \
    static T2 T1##test(...);                                 \
                                                             \
    using _##T1 = decltype(T1##test(nullptr));               \
public:                                                      \

// pointer: Alloc::pointer if present, otherwise value_type*
ALIAS_USING_SFINAE(pointer, value_type*)
    using pointer = _pointer;

// const_pointer: Alloc::const_pointer if present,
//                otherwise std::cyy::pointer_traits<pointer>::rebind<const value_type>
ALIAS_USING_SFINAE(const_pointer, typename cyy::pointer_traits<pointer>::template rebind<const value_type>)
    using const_pointer = _const_pointer;

// void_pointer: Alloc::void_pointer if present, otherwise std::cyy::pointer_traits<pointer>::rebind<void>
ALIAS_USING_SFINAE(void_pointer, typename cyy::pointer_traits<pointer>::template rebind<void>)
    using void_pointer = _void_pointer;

// const_void_pointer: Alloc::const_void_pointer if present, otherwise std::cyy::pointer_traits<pointer>::rebind<const void>
ALIAS_USING_SFINAE(const_void_pointer, typename cyy::pointer_traits<pointer>::template rebind<const void>)
    using const_void_pointer = _const_void_pointer;

// different_type: Alloc::difference_type if present, otherwise std::cyy::pointer_traits<pointer>::difference_type
ALIAS_USING_SFINAE(difference_type, typename cyy::pointer_traits<pointer>::difference_type)
    using difference_type = _difference_type;

// size_type: Alloc::size_type if present, otherwise std::make_unsigned<difference_type>::type
ALIAS_USING_SFINAE(size_type, typename std::make_unsigned<difference_type>::type)
    using size_type = _size_type;

//propagate_on_container_copy_assignment: Alloc::propagate_on_container_copy_assignment if present, otherwise std::false_type
ALIAS_USING_SFINAE(propagate_on_container_copy_assignment, std::false_type)
    using propagate_on_container_copy_assignment = _propagate_on_container_copy_assignment;

// propagate_on_container_move_assignment: Alloc::propagate_on_container_move_assignment if present, otherwise std::false_type
ALIAS_USING_SFINAE(propagate_on_container_move_assignment, std::false_type)
    using propagate_on_container_move_assignment = _propagate_on_container_move_assignment;

// propagate_on_container_swap: Alloc::propagate_on_container_swap if present, otherwise std::false_type
ALIAS_USING_SFINAE(propagate_on_container_swap, std::false_type)
    using propagate_on_container_swap = _propagate_on_container_swap;

    // Alloc::rebind<T>::other if present, otherwise Alloc<T, Args> if this Alloc is Alloc<U, Args>
    template<typename T>
    using rebind_alloc = typename rebind_alloc_helper<Alloc, T>::type;

    template<typename T>
    using rebind_traits = Allocator_traits<rebind_alloc<T>>;

private:
    // allocate_helper, use SFINAE
    template<typename Alloc2>
    struct allocate_helper
    {
        template<typename Alloc3, typename = decltype(std::declval<Alloc2*>()->
            allocate(std::declval<size_type>(), std::declval<const_void_pointer>()))>
        static constexpr
        std::true_type test(int);

        template<typename>
        static constexpr
        std::false_type test(...);

        using type = decltype(test<Alloc2>(0));
    };

    template<typename Alloc2, typename = std::enable_if_t<allocate_helper<Alloc2>::type::value>>
    static pointer allocate_impl(Alloc2& a, size_type n, const_void_pointer hint)
    {
        return a.allocate(n, hint);
    }

    template<typename Alloc2, typename T, typename = std::enable_if_t<!allocate_helper<Alloc2>::type::value>>
    static pointer allocate_impl(Alloc2& a, size_type n, T)
    {
        return a.allocate(n);
    }

    // construct_helper, use SFINAE
    template<typename Alloc2, typename T2, typename... Args2>
    struct construct_helper
    {
        template<typename Alloc3, typename = decltype(std::declval<Alloc3>().
            construct(std::declval<T2*>(), std::declval<Args2>()...))>
        static std::true_type test(int);

        template<typename>
        static std::false_type test(...);

        using type =  decltype(test<Alloc2>(0));
    };

    template<typename T, typename... Args>
    static std::enable_if_t<construct_helper<Alloc, T, Args...>::type::value>
    construct_impl(Alloc& a, T* p, Args&&... args)
    {
        a.construct(p, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    static std::enable_if_t<!construct_helper<Alloc, T, Args...>::type::value&& std::is_constructible_v<T, Args...>>
    construct_impl(Alloc& a, T* p, Args&&... args)
    {
        ::new(static_cast<void*>(p)) T(std::forward<Args>(args)...);
    }

    // destroy_helper, use SFINAE
    template<typename Alloc2, typename T2>
    struct destroy_helper
    {
        template<typename Alloc3, typename = decltype(std::declval<Alloc3>().
            destroy(std::declval<T2*>()))>
        static std::true_type test(int);

        template<typename>
        static std::false_type test(...);

        using type = decltype(test<Alloc2>(0));
    };

    template<typename T>
    static std::enable_if_t<destroy_helper<Alloc, T>::type::value>
    destroy_impl(Alloc& a, T* p)
    {
        a.destroy(p);
    }

    template<typename T>
    static std::enable_if_t<!destroy_helper<Alloc, T>::type::value>
    destroy_impl(Alloc& a, T* p)
    {
        p->~T();
    }

    // max_size_helper, use SFINAE
    template<typename Alloc2>
    struct max_size_helper
    {
        template<typename Alloc3, typename = decltype(std::declval<Alloc2>().
            max_size())>
        static std::true_type test(int);

        template<typename>
        static std::false_type test(...);

        using type = decltype(test<Alloc2>(0));
    };

    template<typename Alloc2, typename = std::enable_if_t<max_size_helper<Alloc2>::type::value>>
    static size_type max_size_impl(const Alloc& a, int)
    {
        return a.max_size();
    }

    template<typename Alloc2, typename = std::enable_if_t<!max_size_helper<Alloc2>::type::value>>
    static size_type max_size_impl(const Alloc& a, ...)
    {
        return std::numeric_limits<size_type>::max();
    }

    // select_helper, use SFINAE
    template<typename Alloc2>
    struct select_helper
    {
        template<typename Alloc3, typename = decltype(std::declval<Alloc3>().
            select_on_container_copy_construction())>
        static std::true_type test(int);

        template<typename>
        static std::false_type test(...);

        using type = decltype(test<Alloc2>(0));
    };

    template<typename Alloc2, typename = std::enable_if_t<select_helper<Alloc2>::type::value>>
    static Alloc select_impl(const Alloc2 &a, int)
    {
        return a.select_on_container_copy_construction();
    }

    template<typename Alloc2, typename = std::enable_if_t<!select_helper<Alloc2>::type::value>>
    static Alloc select_impl(const Alloc2 &a, ...)
    {
        return a;
    }

public:
    // allocates uninitialized storage using the allocator 
    static pointer allocate(Alloc& a, size_type n)
    {
        return a.allocate(n);
    }

    /// Calls a.allocate(n, hint) if possible. If not possible (e.g. a has no two-argument member function allocate()), 
    /// calls a.allocate(n)
    static pointer allocate(Alloc& a, size_type n, const_void_pointer hint)
    {
        return allocate_impl(a, n, hint);
    }

    // Uses the allocator a to deallocate the storage referenced by p, by calling a.deallocate(p, n)
    static void deallocate(Alloc& a, pointer p, size_type n)
    {
        a.deallocate(p, n);
    }

    // If possible, constructs an object of type T in allocated uninitialized storage pointed to by p, by calling
    // a.construct(p, std::forward<Args>(args)...).
    // If the above is not possible (e.g. a does not have the member function construct(),), then calls placement-new as
    // ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...).
    template<typename T, typename... Args>
    static void construct(Alloc& a, T* p, Args&&... args)
    {
        construct_impl(a, p, std::forward<Args>(args)...);
    }

    // Calls the destructor of the object pointed to by p. If possible, does so by calling a.destroy(p). 
    // If not possible (e.g. a does not have the member function destroy()), 
    // then calls the destructor of *p directly, as p->~T(). 
    template<typename T>
    static void destroy(Alloc& a, T* p)
    {
        destroy_impl(a, p);
    }

    // If possible, obtains the maximum theoretically possible allocation size from the allocator a, by calling
    // a.max_size()
    // If the above is not possible (e.g. a does not have the member function max_size()), 
    // then returns std::numeric_limits<size_type>::max() 
    static size_type max_size(const Alloc& a)
    {
        return max_size_imp(a, 0);
    }

    // If possible, obtains the copy-constructed version of the allocator a, 
    // by calling a.select_on_container_copy_construction(). 
    // If the above is not possible (e.g. a does not have the member function select_on_container_copy_construction(),
    //  then returns a, unmodified. 
    static Alloc select_on_container_copy_construction(const Alloc& a)
    {
        return select_impl(a, 0);
    }
};
} // namespace cyy
#endif // ALLOCATOR_TRAITS_H