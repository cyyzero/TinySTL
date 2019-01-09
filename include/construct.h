#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <type_traits>
#include <iterator>
#include "allocator_traits.h"
#include "allocator.h"

namespace cyy
{
// construct an object in existing memory
template<typename T1, typename... Args>
inline void Construct(T1* p, Args&&... args)
{
    ::new(static_cast<void*>(p)) T1(std::forward<Args>(args)...);
}

// destroy an object in existing memory
template<typename T>
inline void Destroy(T* p)
{
    p->~T();
}

template<bool>
struct Destroy_aux
{
    template<typename ForwardIterator>
    static void destroy(ForwardIterator first, ForwardIterator last)
    {
        for (; first != last; ++first)
        {
            Destroy(std::addressof(*first));
        }
    }
};

template<>
struct Destroy_aux<true>
{
    template<typename ForwardIterator>
    static void destroy(ForwardIterator, ForwardIterator)
    {
        // do nothing
    }
};

template<typename ForwardIterator>
void Destroy(ForwardIterator first, ForwardIterator last)
{
    using Value_type = typename std::iterator_traits<ForwardIterator>::value_type;
    Destroy_aux<std::is_trivially_destructible_v<Value_type>>::destroy(first, last);
}

template<typename ForwardIterator, typename Allocator>
void Destroy(ForwardIterator first, ForwardIterator last, Allocator& alloc)
{
    for (; first != last; ++first)
    {
        Allocator_traits<Allocator>::destroy(alloc, std::addressof(*first));
    }
}

template<typename ForwardIterator, typename T>
void Destroy(ForwardIterator first, ForwardIterator last, Allocator<T>&)
{
    Destroy(first, last);
}

} // namespace cyy
#endif // CONSTRUCT_H