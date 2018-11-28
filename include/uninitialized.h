#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

#include "allocator_traits.h"

namespace cyy
{
// construct n elements starting with first allocated by alloc
template<typename ForwardIt, typename Size, typename Alloc>
ForwardIt uninitialized_default_n_a(ForwardIt first, Size n, Alloc& alloc)
{
    // TODO: handle exception
    ForwardIt cur = first;
    for (; n > 0; --n, ++cur)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*cur));
    }
    return cur;
}

// construct n elements starting with first allocated by alloc
template<typename ForwardIt, typename Size, typename Value, typename Alloc>
ForwardIt uninitialized_fill_n_a(ForwardIt first, Size n, const Value& value, Alloc& alloc)
{
    // TODO: handle exception
    ForwardIt cur = first;
    for (; n >0; --n, ++cur)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*cur), value);
    }
    return cur;
}

// construct elements from src to dest allocated by alloc
template<typename InputIt, typename ForwardIt, typename Alloc>
ForwardIt uninitialized_copy_a(InputIt first, InputIt last,
                               ForwardIt target, Alloc& alloc)
{
    // TODO: handle exception
    for (; first != last; ++first, ++target)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*target), *first);
    }
    return target;
}

template<typename InputIt, typename ForwardIt, typename Alloc>
ForwardIt uninitialized_move_a(InputIt first, InputIt last,
                               ForwardIt target, Alloc& alloc)
{
    // TODO: handle excetption
    for (; first != last; ++first, ++target)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*target), std::move(*first));
    }
    return target;
}

}
#endif