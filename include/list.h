#ifndef LIST_H
#define LIST_H

#include "allocator.h"
#include "aligned_buffer.h"
#include "allocator_traits.h"

namespace cyy
{
namespace detail
{

struct List_node_base
{
    List_node_base()
        : prev(nullptr), next(nullptr)
    {
    }

    void hook(List_node_base *node) noexcept
    {
        prev = node->prev;
        next = node;
        node->prev->next = this;
        node->prev = this;
    }

    // void unhook()

    List_node_base *prev;
    List_node_base *next;
};

template <typename T>
struct List_node : public List_node_base
{
    template<typename... Args>
    List_node(Args&&... args)
      : List_node_base(), data(std::forward<Args>(args)...)
    {
    }

    T data;
};

// iterator
template <typename T>
class List_iterator
{
public:

    using self = List_iterator<T>;
    using node_type = List_node<T>;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    List_iterator() noexcept
        : node(nullptr)
    {
    }

    List_iterator(const List_iterator &it)
        : node(it.node)
    {
    }

    explicit
    List_iterator(List_node_base *n)
        : node(n)
    {
    }

    List_iterator &operator=(const List_iterator &rhs)
    {
        node = rhs.node;
    }

    reference operator*() const
    {
        return static_cast<node_type *>(node)->data;
    }

    pointer operator->() const
    {
        return std::addressof(static_cast<node_type *>(node)->data);
    }

    self &operator++()
    {
        node = node->next;
    }

    List_iterator operator++(int)
    {
        auto tmp = *this;
        node = node->next;
        return tmp;
    }

    List_iterator &operator--()
    {
        node = node->prev;
        return *this;
    }

    List_iterator operator--(int)
    {
        auto tmp = *this;
        node = node->prev;
        return tmp;
    }

    List_node_base *node;
};

template <typename T>
class List_const_iterator
{
public:

    using self = List_const_iterator<T>;
    using node_type = List_node<T>;
    using iterator = List_iterator<T>;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;

    List_const_iterator() noexcept
        : node(nullptr)
    {
    }

    List_const_iterator(const List_const_iterator &it) noexcept
        : node(it.node)
    {
    }

    explicit
    List_const_iterator(List_node_base *n) noexcept
        : node(n)
    {
    }

    List_const_iterator &operator=(const List_const_iterator &rhs) noexcept
    {
        node = rhs.node;
    }

    reference operator*() const
    {
        return static_cast<const node_type *>(node)->data;
    }

    pointer operator->() const
    {
        return std::addressof(static_cast<const node_type *>(node)->data);
    }

    self& operator++()
    {
        node = node->next;
        return *this;
    }

    self operator++(int)
    {
        auto tmp = *this;
        node = node->next;
        return tmp;
    }

    self& operator--()
    {
        node = node->prev;
        return *this;
    }

    self operator--(int)
    {
        auto tmp = *this;
        node = node->prev;
        return tmp;
    }

    const List_node_base *node;
};

// compare between List_iterator and List_const_iterator
#define LIST_COMPARE1(OPER, lhs_type, rhs_type)                                \
    template <typename T>                                                      \
    inline bool operator OPER (const lhs_type<T> &lhs, const rhs_type<T> &rhs) \
    {                                                                          \
        return lhs.node OPER rhs.node;                                         \
    }

#define LIST_COMPARE2(lhs_type, rhs_type) \
    LIST_COMPARE1(==, lhs_type, rhs_type) \
    LIST_COMPARE1(!=, lhs_type, rhs_type)

#define LIST_COMPARE3(type1, type2) \
    LIST_COMPARE2(type1, type1)     \
    LIST_COMPARE2(type1, type2)     \
    LIST_COMPARE2(type2, type1)     \
    LIST_COMPARE2(type2, type2)

    LIST_COMPARE3(List_iterator, List_const_iterator)

#undef LISt_COMPARE3
#undef LIST_COMPARE2
#undef LIST_COMPARE1

template <typename T, typename Alloc>
class List_base
{
protected:

    using node_alloc_type = typename Alloc::template rebind<List_node<T>>::other;
    using value_alloc_type = typename Alloc::template rebind<T>::other;

    class List_base_impl : public node_alloc_type
    {
    public:

        List_base_impl()
          : node_alloc_type(), node(0)
        {
        }

        List_base_impl(const Alloc& alloc)
          : Alloc(alloc), node(0)
        {
        }

        List_base_impl(Alloc&& alloc)
          : Alloc(std::move(alloc)), node(0)
        {
        }

        List_node<size_t> node;
    };

    size_t get_size() const
    {
        return head.node.data;
    }

    void set_size(size_t n)
    {
        head.node.data = n;
    }

    void inc_size(size_t n)
    {
        head.node.data += n;
    }

    void dec_size(size_t n)
    {
        head.node.data-= n;
    }

    List_base_impl head;
};
} // namespace detail

template <typename T, typename Alloc = Allocator<T>>
class List : public detail::List_base<T, Alloc>
{
public:

private:

};
} // namespace cyy

#endif // LIST_H
