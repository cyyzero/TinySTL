#ifndef VECTOR_H
#define VECTOR_H

#include "allocator.h"
#include "allocator_traits.h"

namespace cyy
{
template<typename T, typename Alloc>
class Vector_base
{
protected:

};

template<typename T, typename Alloc = cyy::allocator<T>>
class Vector
{
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename cyy::allocator_traits<Alloc>::pointer;
    using const_pointer = typename cyy::allocator_traits<Alloc>::const_pointer;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    
}

} // namespace cyy


#endif // !VECTOR_H
