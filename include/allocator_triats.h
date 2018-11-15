#ifndef ALLOCATOR_TRAITS_H
#define ALLOCATOR_TRAITS_H

namespace cyy
{
template<typename Alloc>
struct allocator_traits
{
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;

// helper macro using SFINAE, define T1 as Alloc::T1 if present, otherwise T2
#define ALIAS_USING_SFINAE(T1, T2) \
private: \
    template<T> \
    static T::##T1 T1##test(T::##T1); \
    static T2 T1##test(...); \
    using _T1 = decltype(test<Alloc>(0)); \
public: \

// pointer: Alloc::pointer if present, otherwise value_type*
ALIAS_USING_SFINAE(pointer, value_type*)
    using pointer = _pointer;

// const_pointer: Alloc::const_pointer if present,
//                otherwise pointer_traits<pointer>::rebind<const value_type>
ALIAS_USING_SFINAE(const_pointer, pointer_traits<pointer>::rebind<const value_type>)
    using const_pointer = _const_pointer;

// void_pointer: Alloc::void_pointer if present, otherwise std::pointer_traits<pointer>::rebind<void>
ALIAS_USING_SFINAE(void_pointer, pointer_traits<pointer>::rebind<void>)
    using void_pointer = _void_pointer;

// const_void_pointer: Alloc::const_void_pointer if present, otherwise std::pointer_traits<pointer>::rebind<const void>
ALIAS_USING_SFINAE(const_void_pointer, pointer_traits<pointer>::rebind<const void>)
    using const_void_pointer = _const_void_pointer;

// different_type: Alloc::difference_type if present, otherwise std::pointer_traits<pointer>::difference_type
ALIAS_USING_SFINAE(difference_type, pointer_traits<pointer>::difference_type)
    using difference_type = _difference_type;

// size_type: Alloc::size_type if present, otherwise std::make_unsigned<difference_type>::type
ALIAS_USING_SFINAE(size_type, std::make_unsigned<difference_type>::type)
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



    static pointer allocate(Alloc& a, size_type n);
    static pointer allocate(Alloc& a, size_type n, const_void_pointer hint);
};
} // namespace cyy
#endif // ALLOCATOR_TRAITS_H