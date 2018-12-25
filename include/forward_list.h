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
    using Self = Fwd_list_iterator<T>;
    using Node = Fwd_list_node<T>;

    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    Fwd_list_iterator() noexcept
        : node()
    {
    }

    explicit
    Fwd_list_iterator(const Self& it) noexcept
        : node(it.node)
    {
    }

    explicit
    Fwd_list_iterator(const Node* n) noexcept
        : node(n)
    {
    }

    reference operator*() const noexcept
    {
        return node->data;
    }

    pointer operator->() const noexcept
    {
        return &node->data;
    }

    Self& operator++() noexcept
    {
        node = node->next;
        return *this;
    }

    Self operator++(int) noexcept
    {
        Self last(*this);
        node = node->next;
        return last;
    }

    bool operator==(const Self& it) const noexcept
    {
        return (node == it.node);
    }

    bool operator!=(const Self& it) const noexcept
    {
        return (node != it.node);
    }

    Self next() const noexcept
    {
        if (node)
        {
            return Self(node->next);
        }
        else
        {
            return Self(nullptr);
        }
    }

    Node *node;
};

template<typename T>
struct Fwd_list_const_iterator
{
    using Node     = Fwd_list_node<T>;
    using Self     = Fwd_list_const_iterator<T>;
    using Iterator = Fwd_list_iterator<T>;

    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    Fwd_list_const_iterator() noexcept
        : node()
    {
    }

    Fwd_list_const_iterator(const Self& it) noexcept
        : node(it.node)
    {
    }

    Fwd_list_const_iterator(const Iterator& it) noexcept
        : node(it.node)
    {
    }

    explicit
    Fwd_list_const_iterator(const Node* n) noexcept
        : node(n)
    {
    }

    reference operator*() const noexcept
    {
        return node->data;
    }

    pointer operator->() const noexcept
    {
        return &node->data;
    }

    Self& operator++() noexcept
    {
        node = node->next;
        return *this;
    }

    Self operator++(int) noexcept
    {
        Self last(*this);
        node = node->next;
        return last;
    }

    bool operator==(const Self& it) const noexcept
    {
        return (node == it.node);
    }

    bool operator!=(const Self& it) const noexcept
    {
        return (node != it.node);
    }

    Self next() const noexcept
    {
        if (node)
        {
            return Self(node->next);
        }
        else
        {
            return Self(nullptr);
        }
    }

   const Node *node;
};

template<typename T>
inline
bool operator==(const Fwd_list_iterator<T>& lhs,
                const Fwd_list_const_iterator<T>& rhs)
{
    return lhs.node == rhs.node;
}

template<typename T>
inline
bool operator!=(const Fwd_list_iterator<T>& lhs,
                const Fwd_list_const_iterator<T>& rhs)
{
    return lhs.node != rhs.node;
}

template<typename T>
inline
bool operator==(const Fwd_list_const_iterator<T>& lhs, 
                const Fwd_list_iterator<T>& rhs)
{
    return lhs.node == rhs.node;
}

template<typename T>
inline
bool operator!=(const Fwd_list_const_iterator<T>& lhs,
                const Fwd_list_iterator<T>& rhs)
{
    return lhs.node != rhs.node;
}

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
    using Base             = detail::Fwd_list_base<T, Allocator>;
    using Node             = detail::Fwd_list_node<T>;
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
    using iterator        = detail::Fwd_list_iterator<T>;
    using const_iterator  = detail::Fwd_list_const_iterator<T>;

}; // class Forward_list

} // namespace cyy

#endif // FORWARD_LIST_H