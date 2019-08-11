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
        return *this;
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
    List_const_iterator(const List_node_base *n) noexcept
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

    using node_type         = List_node<T>;
    using node_alloc_type   = typename Alloc::template rebind<List_node<T>>::other;
    using value_alloc_type  = typename Alloc::template rebind<T>::other;
    using alloc_traits      = cyy::Allocator_traits<Alloc>;
    using node_alloc        = typename alloc_traits:: template rebind_alloc<List_node<T>>;
    using node_alloc_traits = typename alloc_traits:: template rebind_traits<List_node<T>>;

    static
    size_t distance(List_node_base* first, List_node_base* last)
    {
        size_t n = 0;
        while (first != last)
        {
            first = first->next;
            ++n;
        }
        return n;
    }

    class List_base_impl : public node_alloc_type
    {
    public:

        List_base_impl()
          : node_alloc_type(), node(0)
        {
        }

        List_base_impl(const node_alloc_type& alloc)
          : node_alloc_type(alloc), node(0)
        {
        }

        List_base_impl(node_alloc_type&& alloc)
          : node_alloc_type(std::move(alloc)), node(0)
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

    node_type* get_node()
    {
        return node_alloc_traits::allocate(get_node_allocator(), 1);
    }

    void put_node(List_node<T>* p)
    {
        node_alloc_traits::deallocate(get_node_allocator(), p, 1);
    }

    template<typename... Args>
    node_type* create_node(Args&&... args)
    {
        node_type* p = get_node();
        try
        {
            node_alloc_traits::construct(get_node_allocator(), p, std::forward<Args>(args)...);
            return p;
        }
        catch(...)
        {
            put_node(p);
            throw;
        }
    }

    void clear()
    {
        List_node_base* tail = std::addressof(head.node);
        List_node_base* curr = head.node.next;
        while (curr != tail)
        {
            auto tmp = curr->next;
            node_alloc_traits::destroy(get_node_allocator(), static_cast<List_node<T>*>(curr));
            put_node(static_cast<List_node<T>*>(curr));
            curr = tmp;
        }
    }

    List_base_impl head;

public:

    using alloc_type = Alloc;

    node_alloc_type& get_node_allocator()
    {
        return *static_cast<node_alloc_type*>(&head);
    }

    const node_alloc_type& get_node_allocator() const
    {
        return *static_cast<const node_alloc_type*>(&head);
    }

    value_alloc_type get_value_allocator() const
    {
        return value_alloc_type(get_node_allocator());
    }

    alloc_type get_allocator() const
    {
        return alloc_type(get_node_allocator());
    }

    List_base()
      : head()
    {
        init();
    }

    List_base(const node_alloc_type& alloc) noexcept
      : head(alloc)
    {
        init();
    }

    List_base(List_base&& other) noexcept
      : head(std::move(other.get_node_allocator()))
    {
        move_ctor_impl(std::move(other));
    }

    List_base(List_base&& other, const node_alloc_type& alloc) noexcept
      : head(alloc)
    {
        move_ctor_impl(std::move(other));
    }

    ~List_base() noexcept
    {
        clear();
    }

private:
    void init()
    {
        head.node.prev = &head.node;
        head.node.next = &head.node;
    }

    void move_ctor_impl(List_base&& other)
    {
        List_node_base* p = std::addressof(other.head.node);
        if (p->next == p)
            init();
        else
        {
            head.node.prev = p->prev;
            head.node.next = p->next;
            head.node.next->prev = head.node.prev->next = std::addressof(head.node);
            set_size(other.get_size());
            other.init();
        }
    }

};
} // namespace detail

template <typename T, typename Alloc = Allocator<T>>
class List : protected detail::List_base<T, Alloc>
{
    using base_type         = detail::List_base<T, Alloc>;
    using value_alloc_type  = typename base_type::value_alloc_type;
    using node_alloc_type   = typename base_type::node_alloc_type;
    using node_alloc_traits = typename base_type::node_alloc_traits;
    using alloc_value_type  = typename Alloc::value_type;
    using node_type         = detail::List_node<T>;
    using node_base_type    = detail::List_node_base;

    using base_type::head;
    using base_type::put_node;
    using base_type::get_node;
    using base_type::get_value_allocator;
    using base_type::get_node_allocator;
    using base_type::create_node;
    using base_type::inc_size;
    using base_type::dec_size;
    using base_type::set_size;
    using base_type::clear;

public:
    using value_type             = T;
    using allocator_type         = Alloc;
    using size_type              = size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pinter                 = typename Allocator_traits<Alloc>::pointer;
    using const_pointer          = typename Allocator_traits<Alloc>::const_pointer;
    using iterator               = detail::List_iterator<T>;
    using const_iterator         = detail::List_const_iterator<T>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // constructors
    List()
      : base_type()
    {
    }

    explicit
    List(const allocator_type& alloc) noexcept
      : base_type(node_alloc_type(alloc))
    {
    }

    explicit
    List(size_t count, const allocator_type& alloc = allocator_type())
      : base_type(node_alloc_type(alloc))
    {
        default_initialize(count);
    }

    List(size_t count, const value_type& value, const allocator_type& alloc = allocator_type())
      : base_type(node_alloc_type(alloc))
    {
        fill_initialize(count, value);
    }

    template<typename InputIt, typename = typename std::enable_if<std::is_convertible<
        typename std::iterator_traits<InputIt>::iterator_category,
        std::input_iterator_tag>::value>::type>
    List(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
      : base_type(node_alloc_type(alloc))
    {
        range_initialize(first, last);
    }

    List(const List& other)
      : base_type(node_alloc_type(other.get_node_allocator()))
    {
        range_initialize(other.begin(), other.end());
    }

    List(const List& other, const allocator_type& alloc)
      : base_type(node_alloc_type(alloc))
    {
        range_initialize(other.begin(), other.end());
    }

    List(List&& other)
      : base_type(std::move(other))
    {
    }

    List(List&& other, const allocator_type& alloc)
      : base_type(std::move(other), node_alloc_type(alloc))
    {
    }

    List(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
      : List(node_alloc_type(alloc))
    {
        range_initialize(init.begin(), init.end());
    }

    // assign values to the container 
    List& operator=(const List& other)
    {
        
    }

    // return the allocator associated with the container
    allocator_type get_allocator() const
    {
        return base_type::get_allocator();
    }

    // access the first element
    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    // access the last element
    reference back()
    {
        auto tmp = end();
        --tmp;
        return *tmp;
    }

    const_reference back() const
    {
        auto tmp = end();
        --tmp;
        return *tmp;
    }

    // return an iterator to the beginning
    iterator begin() noexcept
    {
        return iterator(head.node.next);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(head.node.next);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(head.node.next);
    }

    // return an iterator to the end
    iterator end() noexcept
    {
        return iterator(&head.node);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(&head.node);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(&head.node);
    }

    // return a reverse iterator to the beginning
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    // return a reverse iterator to the end
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    // check whether the container is empty
    bool empty() const noexcept
    {
        return head.node.next == &head.node;
    }

    // return the number of elements
    size_type size() const noexcept
    {
        return head.node.data;
    }

    // return the maximum possible number of elements
    size_type max_size() const noexcept
    {
        return node_alloc_traits::max_size(get_node_allocator());
    }

    // clear the contents
    void clear() noexcept
    {
        base_type::clear();
        set_size(0);
    }

    // add an element to the end
    void push_back(const value_type& value)
    {
        emplace_back(value);
    }

    void push_back(value_type&& value)
    {
        emplace_back(std::move(value));
    }

    // construct an element in-place at the end
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        auto n = create_node(std::forward<Args>(args)...);
        auto prev = head.node.prev;
        head.node.prev = n;
        n->next = &head.node;
        prev->next = n;
        n->prev = prev;
        inc_size(1);
    }

    void swap(List& other)
    {
        std::swap(other.head.node, head.node);
    }

private:
    void default_initialize(size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            emplace_back();
        }
    }

    void fill_initialize(size_t count, const value_type& value)
    {
        for (size_t i = 0; i < count; ++i)
        {
            push_back(value);
        }
    }

    template<typename InputIt>
    void range_initialize(InputIt first, InputIt last)
    {
        while (first != last)
        {
            emplace_back(*first);
            ++first;
        }
    }

};

template<typename T, typename Alloc>
void swap(List<T,Alloc>& lhs,
          List<T,Alloc>& rhs)
{
    lhs.swap(rhs);
}
} // namespace cyy

#endif // LIST_H
