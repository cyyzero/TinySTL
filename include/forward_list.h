#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <type_traits>
#include "allocator.h"
#include "allocator_traits.h"

namespace cyy
{
namespace detail
{
// aligned buffer used in Fwd_list_node
template<typename T>
struct aligned_buffer
    : public std::aligned_storage<sizeof(T), alignof(T)>
{
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;

    aligned_buffer() = default;

    void* address() noexcept
    {
        return static_cast<void*>(&storage);
    }

    const void* address() const noexcept
    {
        return static_cast<const void*>(&storage);
    }

    T* pointer() noexcept
    {
        return static_cast<T*>(address());
    }

    const T* pointer() const noexcept
    {
        return static_cast<const T*>(address());
    }

}; // class aligned_buffer

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

    T* valptr()
    {
        return storage.pointer();
    }

    const T* valptr() const
    {
        return storage.pointer();
    }

    aligned_buffer<T> storage;
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

    Fwd_list_iterator(const Self& it) noexcept
        : node(it.node)
    {
    }

    explicit
    Fwd_list_iterator(Fwd_list_node_base *n) noexcept
        : node(n)
    {
    }

    reference operator*() const noexcept
    {
        return *static_cast<Node*>(node)->valptr();
    }

    pointer operator->() const noexcept
    {
        return static_cast<Node*>(node)->valptr();
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

    using value_type        = T;
    using reference         = const T&;
    using pointer           = const T*;
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
        return *static_cast<const Node*>(node)->valptr();
    }

    pointer operator->() const noexcept
    {
        return static_cast<const Node*>(node)->valptr();
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
    using Node_alloc        = typename Alloc_traits:: template rebind_alloc<Node>;
    using Node_alloc_traits = typename Alloc_traits:: template rebind_traits<Node>;

    using iterator          = Fwd_list_iterator<T>;
    using const_iterator    = Fwd_list_const_iterator<T>;

protected:
    struct Fwd_list_impl 
        : public Node_alloc
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
            : Node_alloc(std::move(alloc)), head()
        {
        }

        ~Fwd_list_impl() = default;

        Fwd_list_node_base head;
    };

    Fwd_list_impl head_impl;

public:
    Fwd_list_base()
        : head_impl()
    {
    }

    Fwd_list_base(const Node_alloc& alloc)
        : head_impl(alloc)
    {
    }

    Fwd_list_base(Fwd_list_base&& list, const Node_alloc& alloc)
        : head_impl(alloc)
    {
        if (list.get_node_allocator() == alloc)
        {
            head_impl.head.next = list.head_impl.head.next;
            list.head_impl.head.next = nullptr;
        }
        else
        {
            head_impl.head.next = nullptr;
            Fwd_list_node_base *to = &this->head_impl.head;
            Node *curr = static_cast<Node*>(list.head_impl.head.next);

            while (curr)
            {
                to->next = create_node(std::move(*curr->valptr()));
                to = to->next;
                curr = static_cast<Node*>(curr->next);
            }
        }
    }

    Fwd_list_base(Fwd_list_base&& list)
        : head_impl(std::move(list.get_node_allocator()))
    {
        head_impl.head.next = list.head_impl.head.next;
        list.head_impl.head.next = nullptr;
    }

    ~Fwd_list_base()
    {
        erase_after(&head_impl.head, nullptr);
    }

protected:
    template<typename... Args>
    Fwd_list_node_base* insert_after(const_iterator pos, Args&&... args)
    {
        return insert_after(const_cast<Fwd_list_node_base*>(pos.node), std::forward<Args>(args)...);
    }

    template<typename... Args>
    Fwd_list_node_base* insert_after(Fwd_list_node_base* pos, Args&&... args)
    {
        Node* node = create_node(std::forward<Args>(args)...);
        node->next = pos->next;
        pos->next = node;
        return node;
    }

    Fwd_list_node_base* erase_after(Fwd_list_node_base* pos)
    {
        Node* curr= static_cast<Node*>(pos->next);
        pos->next = curr->next;
        Allocator alloc(get_node_allocator());
        cyy::Allocator_traits<Allocator>::destroy(alloc, curr->valptr());
        Node_alloc_traits::destroy(get_node_allocator(), curr);
        put_node(curr);
        return curr->next;
    }

    Fwd_list_node_base* erase_after(Fwd_list_node_base* pos, Fwd_list_node_base* last)
    {
        Node* curr = static_cast<Node*>(pos->next);
        Allocator alloc(get_node_allocator());
        while (curr != last)
        {
            Node* tmp = static_cast<Node*>(curr->next);
            cyy::Allocator_traits<Allocator>::destroy(alloc, curr->valptr());
            Node_alloc_traits::destroy(get_node_allocator(), curr);
            put_node(curr);
            curr = tmp;
        }
        pos->next = last;
        return last;
    }

    Node_alloc& get_node_allocator() noexcept
    {
        return static_cast<Node_alloc&>(head_impl);
    }

    const Node_alloc& get_node_allocator() const noexcept
    {
        return static_cast<const Node_alloc&>(head_impl);
    }

    Node* get_node()
    {
        auto ptr = Node_alloc_traits::allocate(get_node_allocator(), 1);
        return std::addressof(*ptr);
    }

    void put_node(Node* p)
    {
        using Ptr = typename Node_alloc_traits::pointer;
        auto ptr = cyy::pointer_traits<Ptr>::pointer_to(*p);
        Node_alloc_traits::deallocate(get_node_allocator(), ptr, 1);
    }

    template<typename... Args>
    Node* create_node(Args&&... args)
    {
        Node* node = get_node();
        try
        {
            Allocator alloc(get_node_allocator());
            Node_alloc_traits::construct(get_node_allocator(), node);
            Alloc_traits::construct(alloc, node->valptr(), std::forward<Args>(args)...);
        }
        catch (...)
        {
            put_node(node);
            throw;
        }
        return node;
    }

}; // class Fwd_list_base
} // namespace detail

template<typename T, typename Allocator = cyy::Allocator<T>>
class Forward_list : public detail::Fwd_list_base<T, Allocator>
{
private:
    using Base            = detail::Fwd_list_base<T, Allocator>;
    using Node            = detail::Fwd_list_node<T>;
    using Node_base       = detail::Fwd_list_node_base;
    using Alloc_traits    = cyy::Allocator_traits<Allocator>;

    using typename Base::Node_alloc;
    using typename Base::Node_alloc_traits;
    using Base::create_node;
    using Base::head_impl;
    using Base::get_node_allocator;
    using Base::erase_after;
    using Base::insert_after;

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

    // constructors
    Forward_list() = default;

    explicit
    Forward_list(const Allocator& alloc)
        : Base(Node_alloc(alloc))
    {
    }

    Forward_list(size_type count, const value_type& value, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
        fill_initialize(count, value);
    }

    explicit
    Forward_list(size_type count)
        : Base(Node_alloc())
    {
        default_initialize(count);
    }

    explicit Forward_list(size_type count, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
        default_initialize(count);
    }

    template<typename InputIterator>
    Forward_list(InputIterator first, InputIterator last, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
        range_initialize(first, last);
    }

    Forward_list(const Forward_list& other)
        : Base(other.get_node_allocator())
    {
        range_initialize(other.begin(), other.end());
    }

    Forward_list(const Forward_list& other, const Allocator& alloc)
        : Base(Node_alloc(alloc))
    {
        range_initialize(other.begin(), other.end());
    }

    Forward_list(Forward_list&& other)
        : Base(std::move(other))
    {
    }

    Forward_list(Forward_list&& other, const Allocator& alloc)
        : Base(std::move(other), Node_alloc(alloc))
    {
    }

    Forward_list(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : Base(Node_alloc(alloc))
    {
        range_initialize(init.begin(), init.end());
    }

    // destructor
    ~Forward_list() = default;

    // operator=
    Forward_list& operator=(const Forward_list& other)
    {
        if (&other == this)
            return *this;

        if (Alloc_traits::propagate_on_container_copy_assignment::value)
        {
            get_node_allocator() = other.get_node_allocator();
        }

        if (get_node_allocator() != other.get_node_allocator())
        {
            erase_after(&head_impl.head, nullptr);
        }
        assign(other.begin(), other.end());
        return *this;
    }

    Forward_list& operator=(Forward_list&& other)
    {
        if (Alloc_traits::propagate_on_container_move_assignment::value)
        {
            get_node_allocator() = other.get_node_allocator();
        }

        if (get_node_allocator() == other.get_node_allocator())
        {
            erase_after(&head_impl.head, nullptr);
            std::swap(head_impl.head.next, other.head_impl.head.next);
        }
        else
        {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    Forward_list& operator=(std::initializer_list<value_type> ilist)
    {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    // assign values to the container 
    void assign(size_type count, const value_type& value)
    {
        size_type i = 0;
        Node_base* it = &head_impl.head;
        for (;it->next != nullptr && i < count; it = it->next, ++i)
        {
            *(static_cast<Node*>(it->next)->valptr()) = value;
        }

        if (i == count)
        {
            erase_after(it, nullptr);
        }
        else
        {
            for (; i < count; ++i)
            {
                it = insert_after(it, value);
            }
        }
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        Node_base* it = &head_impl.head;
        for (; it->next != nullptr && first != last; it = it->next, ++first)
        {
            *(static_cast<Node*>(it)->valptr()) = *first;
        }

        if (first == last)
        {
            erase_after(it, nullptr);
        }
        else
        {
            for (; first != last; ++first)
            {
                it = insert_after(it, *first);
            }
        }
    }

    void assign(std::initializer_list<value_type> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    // returns the associated allocator
    allocator_type get_allocator() const
    {
        return allocator_type(get_node_allocator());
    }

    // access the first element
    reference front()
    {
        return *static_cast<Node*>(head_impl.head.next)->valptr();
    }

    const_reference front() const
    {
        return *static_cast<Node*>(head_impl.head.next)->valptr();
    }

    iterator begin()
    {
        return iterator(head_impl.head.next);
    }

    const_iterator begin() const
    {
        return const_iterator(head_impl.head.next);
    }

    const_iterator cbegin() const
    {
        return const_iterator(head_impl.head.next);
    }

    iterator end()
    {
        return iterator(nullptr);
    }

    const_iterator end() const
    {
        return const_iterator(nullptr);
    }

    const_iterator cend() const
    {
        return const_iterator(nullptr);
    }

private:
    void default_initialize(size_type count)
    {
        Node* curr;
        Node_base* prev = &head_impl.head;
        while (count--)
        {
            curr = create_node();
            prev->next = curr;
            prev = curr;
        }
    }

    void fill_initialize(size_type count, const value_type& value)
    {
        Node* curr;
        Node_base* prev = &head_impl.head;
        while (count--)
        {
            curr = create_node(value);
            prev->next = curr;
            prev = curr;
        }
    }

    template<typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last)
    {
        Node* curr;
        Node_base* prev = &head_impl.head;
        while (first != last)
        {
            curr = create_node(*first);
            prev->next = curr;
            prev = curr;
            ++first;
        }
    }
}; // class Forward_list

} // namespace cyy

#endif // FORWARD_LIST_H