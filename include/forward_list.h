#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include "allocator.h"
#include "allocator_traits.h"

namespace cyy
{
namespace detail
{
// base class of Fwd_list_node
struct Fwd_list_node_base
{
    Fwd_list_node_base()
        : next(nullptr)
    {
    }

    Fwd_list_node_base *next;
};

// node of Forward_list
template<typename T>
struct Fwd_list_node
    : public Fwd_list_node_base
{
    Fwd_list_node() = default;
    ~Fwd_list_node() = default;

    T data;
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
    Fwd_list_iterator(const Fwd_list_node_base *n) noexcept
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

    Fwd_list_node_base *node;
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
    Fwd_list_const_iterator(const Fwd_list_node_base *n) noexcept
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

   const Fwd_list_node_base *node;
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
    using Node              = Fwd_list_node<T>;
    using Alloc_traits      = cyy::Allocator_traits<Allocator>;
    using Node_alloc        = Alloc_traits::rebind_alloc<Node>;
    using Node_alloc_traits = Alloc_traits::rebind_traits<Node>;

protected:
    struct Fwd_list_impl : public Node_alloc
    {
        Fwd_list_impl()
            : Node_alloc(), head()
        {
        }

        Fwd_list_impl(const Node_alloc& alloc)
            : Node_alloc(alloc), head()
        {
        }

        Fwd_list_impl(Node_alloc&& alloc)
            : Node_alloc(std::move(alloc), head()
        {
        }

        ~Fwd_list_impl() = default;

        Fwd_list_node_base head;
    };

    Fwd_list_impl head_impl;

public:
    Fwd_list_base():
        : head_impl()
    {
    }

    Fwd_list_base(const Node_alloc& alloc)
        : head_impl(alloc)
    {
    }

    Fwd_list_base(Fwd_list_base&& list, const Node_alloc& alloc)
    {
    }

    Fwd_list_base(Fwd_list_base&& list)
        : head_impl(std::move(list.get_node_allocator()))
    {
        head_impl.head.next = list.head_impl.head.next;
        list.head_impl.head.next = nullptr;
    }

    ~Fwd_list_base()
    {
        
    }

    Node_alloc& get_node_allocator() noexcept
    {
        return static_cast<Node_alloc&>(head_impl);
    }

    const Node_alloc& get_node_allocator() const noexcept
    {
        return static_cast<const Node_alloc&>(head_impl);
    }
}; // class Fwd_list
} // namespace detail

template<typename T, typename Allocator = cyy::Allocator<T>>
class Forward_list : public Fwd_list_base<T, Allocator>
{
private:
    using Base            = detail::Fwd_list_base<T, Allocator>;
    using Node            = detail::Fwd_list_node<T>;
    using Node_base       = detail::Fwd_list_node_base;
    using Alloc_traits    = cyy::Allocator_traits<Allocator>;
    using Base::Node_alloc;
    using Base::Node_alloc_traits;

public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename Allocator_traits<Allocator>::pointer;
    using const_pointer   = typename Allocator_traits<Allocator>::const_pointer;
    using iterator        = detail::Fwd_list_iterator<T>;
    using const_iterator  = detail::Fwd_list_const_iterator<T>;

    Forward_list() = default;

    explicit Forward_list(const Allocator& alloc)
        : Base(Node_alloc(alloc))
    {
    }

    Forward_list(size_type count, const T& value, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
    }

    explicit Forward_list(size_type count)
        : Base(Node_alloc())
    {
    }

    explicit Forward_list(size_type count, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
    }

    template<class InputIterator>
    Forward_list(InputIterator first, InputIterator last, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
    }

    Forward_list(const Forward_list& other)
        : Base(other.get_node_allocator())
    {
    }

    Forward_list(const Forward_list& other, const Allocator& alloc)
        : Base(Node_alloc(alloc))
    {
    }

    Forward_list(Forward_list&& other)
    {
    }

    Forward_list(Forward_list&& other, const Allocator& alloc)
        : Base(Node_alloc(alloc))
    {
    }

    Forward_list(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
    }
}; // class Forward_list

} // namespace cyy

#endif // FORWARD_LIST_H