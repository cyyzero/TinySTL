#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include "allocator.h"
#include "allocator_traits.h"

namespace cyy
{
namespace detail
{

// node of Forward_list
template<typename T>
struct Fwd_list_node
{
    Fwd_list_node() = default;
    ~Fwd_list_node() = default;

    T data;
    T* next;
};

// iterator
template<typename T>
struct Fwd_list_iterator
{

};

template<typename T>
struct Fwd_list_const_iterator
{

};

// base class for Forward_list
template<typename T, typename Allocator>
struct Fwd_list_base
{
    using Alloc_traits = cyy::allocator_traits<Allocator>;
    // using   cyy::allocator_traits<Allocator>::rebind_alloc<>;


}; // class Fwd_list
} // namespace detail

template<typename T, typename Allocator = allocator<T>>
class Forward_list
{
private:
    using Base             = Fwd_list_base<T, Allocator>;
    using Node             = Fwd_list_node<T>;
    using Node_base        = Fwd_list_node_base;
    using Alloc_traits     = cyy::allocator_traits<Allocator>;

public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename allocator_traits<Allocator>::pointer;
    using const_pointer   = typename allocator_traits<Allocator>::const_pointer;
    using iterator        = ;
    using const_iterator  = ;

}; // class Forward_list

} // namespace cyy

#endif // FORWARD_LIST_H