#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

#include "allocator_triats.h"

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
ForwardIt uninitialized_copy_a(InputIt src_begin, InputIt src_end,
                               ForwardIt dest_begin, Alloc& alloc)
{
    // TODO: handle exception
    ForwardIt cur = dest_begin;

    for (; src_begin != src_end; ++src_begin, ++cur)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*cur), *src_begin);
    }
    return cur;
}

template<typename InputIt, typename ForwardIt, typename Alloc>
ForwardIt uninitialized_move_a(InputIt src_begin, InputIt src_end,
                               ForwardIt dest_begin, Alloc& alloc)
{
    // TODO: handle excetption
    ForwardIt cur = dest_begin;
    for (; src_begin != src_end; ++src_begin, ++cur)
    {
        allocator_traits<Alloc>::construct(alloc, std::addressof(*cur), std::move(*cur));
    }
    return cur;
}

}
#endif