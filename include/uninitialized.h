#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

#include "allocator_traits.h"
#include "construct.h"

namespace cyy
{
// construct n elements starting with first allocated by alloc
template<typename ForwardIterator, typename Size, typename Allocator>
ForwardIterator uninitialized_default_n_a(ForwardIterator first, Size n, Allocator& alloc)
{
    ForwardIterator cur = first;
    try
    {
        for (; n > 0; --n, ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur));
        }
        return cur;
    }
    catch (...)
    {
        cyy::Destroy(first, cur, alloc);
        throw;
    }
}

// construct n elements starting with first allocated by alloc
template<typename ForwardIterator, typename Size, typename Value, typename Allocator>
ForwardIterator uninitialized_fill_n_a(ForwardIterator first, Size n, const Value& value, Allocator& alloc)
{
    ForwardIterator cur = first;
    try
    {
        for (; n > 0; --n, ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur), value);
        }
        return cur;
    }
    catch (...)
    {
        cyy::Destroy(first, cur, alloc);
        throw;
    }
}

// construct value from first to last via Allocator
template<typename ForwardIterator, typename Value, typename Allocator>
void uninitialized_fill_a(ForwardIterator first, ForwardIterator last, const Value& value, Allocator& alloc)
{
    ForwardIterator cur = first;
    try
    {
        for (; cur != last; ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur), value);
        }
    }
    catch (...)
    {
        cyy::Destroy(first, cur, alloc);
        throw;
    }
}

// construct elements from src to dest allocated by alloc
template<typename InputIterator, typename ForwardIterator, typename Allocator>
ForwardIterator uninitialized_copy_a(InputIterator first, InputIterator last,
                                     ForwardIterator target, Allocator& alloc)
{
    ForwardIterator cur = target;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur), *first);
        }
        return cur;
    }
    catch (...)
    {
        cyy::Destroy(target, cur, alloc);
        throw;
    }
}

template<typename InputIterator, typename ForwardIterator, typename Allocator>
ForwardIterator uninitialized_move_a(InputIterator first, InputIterator last,
                                     ForwardIterator target, Allocator& alloc)
{

    ForwardIterator cur = target;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur), std::move(*first));
        }
        return cur;
    }
    catch (...)
    {
        cyy::Destroy(target, cur, alloc);
        throw;
    }
}

template<typename InputIterator, typename ForwardIterator, typename Allocator>
ForwardIterator uninitialized_move_if_noexcept_a(InputIterator first, InputIterator last,
                                     ForwardIterator target, Allocator& alloc)
{

    ForwardIterator cur = target;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            Allocator_traits<Allocator>::construct(alloc, std::addressof(*cur), std::move_if_noexcept(*first));
        }
        return cur;
    }
    catch (...)
    {
        cyy::Destroy(target, cur, alloc);
        throw;
    }
}

}
#endif