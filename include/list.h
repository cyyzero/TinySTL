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

    List_node<T>* get_node()
    {
        return head.node_alloc_type::allocate(1);
    }

    void put_node(List_node<T>* p)
    {
        head.node_alloc_type::deallocate(p, 1);
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
        List_node_base* tail = std::addressof(head.node);
        List_node_base* curr = tail->next;
        while (curr != tail)
        {
            auto tmp = curr->next;
            head.node_alloc_type::deallocate(curr, 1);
            curr = tmp;
        }
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
    using base_type        = detail::List_base<T, Alloc>;
    using value_alloc_type = typename base_type::value_alloc_type;
    using node_alloc_type  = typename base_type::node_alloc_type;
    using alloc_value_type = typename Alloc::value_type;
    using node_type        = detail::List_node<T>;
    using node_base_type   = detail::List_node_base;

    using base_type::head;
    using base_type::put_node;
    using base_type::get_node;
    using base_type::get_value_allocator;
    using base_type::get_node_allocator;

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

    // constructor
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
        fill_initialize(count, value)
    }

    template<typename InputIt, typename = typename std::enable_if<std::is_convertible<std::input_iterator_tag, std::iterator_traits<InputIt>::iterator_category>::value>::type>
    List(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
    {
        // TODO: impl
    }

    List(const List& other)
    {

    }

    List(const List& other, const allocator_type& alloc)
      : base_type(node_alloc_type(alloc))
    {

    }

    List(List&& other)
      : base_type(std::move(other.base_type))
    {

    }

    List(List&& other, const allocator_type& alloc)
      : base_type(std::move(other.base_type), node_alloc_type(alloc))
    {
        
    }

    List(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
      : List(init.begin(), init.end(), alloc)
    {
    }

private:
    template<typename... Args>
    node_type* create_node(Args&&... args)
    {
        node_type* p = get_node();
        try
        {
            get_node_allocator().construct(p, std::forward<Args>(args)...);
            return p;
        }
        catch(...)
        {
            put_node(p);
            throw;
        }
    }

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

};
} // namespace cyy

#endif // LIST_H
