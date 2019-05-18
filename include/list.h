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
    T *valptr()
    {
        return static_cast<T *>(&storage);
    }

    const T *valptr() const
    {
        return static_cast<const T *>(&storage);
    }

    // NEVER use before storage has been constructed as T or T has been destroyed
    T &valref()
    {
        return *valptr();
    }

    const T &valref() const
    {
        return *valptr();
    }

    aligned_buffer<T> storage;
};

// iterator
template <typename T>
class List_iterator
{
public:
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

    T &operator*() const
    {
        return static_cast<List_node<T> *>(node)->valref();
    }

    T *operator->() const
    {
        return static_cast<List_node<T> *>(node)->valptr();
    }

    List_iterator &operator++()
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

    const T &operator*() const
    {
        return static_cast<const List_node<T> *>(node)->valref();
    }

    const T *operator->() const
    {
        return static_cast<const List_node<T> *>(node)->valptr();
    }

    List_const_iterator &operator++()
    {
        node = node->next;
        return *this;
    }

    List_const_iterator operator++(int)
    {
        auto tmp = *this;
        node = node->next;
        return tmp;
    }

    List_const_iterator &operator--()
    {
        node = node->prev;
        return *this;
    }

    List_const_iterator operator--(int)
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
    class List_base_impl : public Alloc
    {
    };

public:
private:
};
} // namespace detail

template <typename T, typename Alloc = Allocator<T>>
class List : public detail::List_base<T, Alloc>
{
};
} // namespace cyy

#endif // LIST_H
