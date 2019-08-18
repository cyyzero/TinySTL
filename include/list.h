#ifndef LIST_H
#define LIST_H

#include "allocator.h"
#include "aligned_buffer.h"
#include "allocator_traits.h"

namespace cyy
{

template <typename T, typename Alloc>
class List_base;
template <typename T, typename Alloc>
class List;

namespace detail
{

struct List_node_base
{
    List_node_base()
        : prev(nullptr), next(nullptr)
    {
    }

    // append this after node
    void hook(List_node_base *node) noexcept
    {
        prev = node;
        next = node->next;
        node->next->prev = this;
        node->next = this;
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

#define ITERATOR_FRIEND_OPERATION1(OPER, LHS_TYPE, RHS_TYPE) \
template<typename U> \
friend bool operator OPER (const LHS_TYPE<U>& lhs, const RHS_TYPE<U>& rhs);

#define ITERATOR_FRIEND_OPERATION2(lhs_type, rhs_type) \
    ITERATOR_FRIEND_OPERATION1(==, lhs_type, rhs_type) \
    ITERATOR_FRIEND_OPERATION1(!=, lhs_type, rhs_type)

#define ITERATOR_FRIEND_OPERATION3(type1, type2) \
    ITERATOR_FRIEND_OPERATION2(type1, type1)     \
    ITERATOR_FRIEND_OPERATION2(type1, type2)     \
    ITERATOR_FRIEND_OPERATION2(type2, type1)     \
    ITERATOR_FRIEND_OPERATION2(type2, type2)

#define ITERATOR_FRIEND_OPERATION \
ITERATOR_FRIEND_OPERATION3(List_iterator, List_const_iterator)

template<typename T>
class List_const_iterator;

template <typename T>
class List_iterator
{
    template <typename U,  typename Alloc>
    friend class cyy::List_base;

    template <typename U,  typename Alloc>
    friend class cyy::List;

    template<typename U>
    friend class List_const_iterator;

    ITERATOR_FRIEND_OPERATION

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
        return *this;
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

private:
    List_node_base *node;
};

template <typename T>
class List_const_iterator
{
    template <typename U,  typename Alloc>
    friend class cyy::List_base;

    template <typename U,  typename Alloc>
    friend class cyy::List;

    ITERATOR_FRIEND_OPERATION

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

    List_const_iterator(const List_iterator<T>& it) noexcept
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
        return *this;
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

private:
    const List_node_base *node;
};

// compare between List_iterator and List_const_iterator
#define LIST_ITERATOR_COMPARE1(OPER, lhs_type, rhs_type)                                \
    template <typename T>                                                      \
    inline bool operator OPER (const lhs_type<T> &lhs, const rhs_type<T> &rhs) \
    {                                                                          \
        return lhs.node OPER rhs.node;                                         \
    }

#define LIST_ITERATOR_COMPARE2(lhs_type, rhs_type) \
    LIST_ITERATOR_COMPARE1(==, lhs_type, rhs_type) \
    LIST_ITERATOR_COMPARE1(!=, lhs_type, rhs_type)

#define LIST_ITERATOR_COMPARE3(type1, type2) \
    LIST_ITERATOR_COMPARE2(type1, type1)     \
    LIST_ITERATOR_COMPARE2(type1, type2)     \
    LIST_ITERATOR_COMPARE2(type2, type1)     \
    LIST_ITERATOR_COMPARE2(type2, type2)

    LIST_ITERATOR_COMPARE3(List_iterator, List_const_iterator)

#undef LIST_ITERATOR_COMPARE3
#undef LIST_ITERATOR_COMPARE2
#undef LIST_ITERATOR_COMPARE1

#undef ITERATOR_FRIEND_OPERATION
#undef ITERATOR_FRIEND_OPERATION1
#undef ITERATOR_FRIEND_OPERATION2
#undef ITERATOR_FRIEND_OPERATION3

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

    void init()
    {
        head.node.prev = &head.node;
        head.node.next = &head.node;
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
    using base_type::init;

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
        if (&other != this)
        {
            range_assign(other.begin(), other.end());
        }
        return *this;
    }

    List& operator=(List&& other)
    {
        swap(other);
        other.clear();
        return *this;
    }

    List& operator=(std::initializer_list<T> ilist)
    {
        range_assign(ilist.begin(), ilist.end());
        return *this;
    }

    // assign values to the container
    void assign(size_type count, const T& value)
    {
        fill_assign(count, value);
    }

    template<typename InputIt, typename = typename std::enable_if<std::is_convertible<
        typename std::iterator_traits<InputIt>::iterator_category,
        std::input_iterator_tag>::value>::type>
    void assign(InputIt first, InputIt last)
    {
        range_assign(first, last);
    }

    void assign(std::initializer_list<T> ilist)
    {
        range_assign(ilist.begin(), ilist.end());
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
        base_type::init();
        set_size(0);
    }

    // insert element(s) before 
    iterator insert(const_iterator pos, const value_type& value)
    {
        iterator it(insert_impl(const_cast<node_base_type*>(pos.node->prev), value));
        inc_size(1);
        return it;
    }

    iterator insert( const_iterator pos, size_type count, const value_type& value)
    {
        node_base_type* p = const_cast<node_base_type*>(pos.node->prev);
        for (size_t i = 0; i < count; ++i)
        {
            p = insert_impl(p, value);
        }
        inc_size(count);
        return iterator(p);
    }

    template<typename InputIt >
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        node_base_type* p = const_cast<node_base_type*>(pos.node->prev);
        size_type count = 0;
        for (; first != last; ++first, ++count)
        {
            p = insert_impl(p, *first);
        }
        inc_size(count);
        return iterator(p);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    // insert a new element into the container directly before pos
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        node_base_type* p = const_cast<node_base_type*>(pos.node->prev);
        p = insert_impl(p, std::forward<Args>(args)...);
        inc_size(1);
        return iterator(p);
    }

    // erase element(s)
    iterator erase(const_iterator pos)
    {
        auto p = static_cast<node_type*>(const_cast<node_base_type*>(pos.node));
        auto next = p->next;
        auto prev = p->prev;
        node_alloc_traits::destroy(get_node_allocator(), p);
        put_node(p);
        next->prev = prev;
        prev->next = next;
        dec_size(1);
        return iterator(next);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        size_t count = 0;
        while (first != last)
        {
            first = erase(first);
            ++count;
        }
        dec_size(count);
        return iterator(const_cast<node_base_type*>(last.node));
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
        emplace(cend(), std::forward<Args>(args)...);
    }

    // remove the last element
    void pop_back()
    {
        erase(const_iterator(head.node.prev));
    }

    // insert an element to the beginning
    void push_front(const value_type& value)
    {
        insert_impl(&head.node, value);
    }

    void push_front(value_type&& value)
    {
        insert_impl(&head.node, std::move(value));
    }

    // insert a new element to the beginning of the container
    template<typename... Args>
    void emplace_front(Args&&... args)
    {
        insert_impl(&head.node, std::forward<Args>(args)...);
    }

    // remove the first element of the container
    void pop_front()
    {
        erase(begin());
    }

    // change the number of elements stored
    void resize(size_type count)
    {
        while (size() > count)
        {
            pop_back();
        }
        while (size() < count)
        {
            emplace_back();
        }
    }

    void resize(size_type count, const value_type& value)
    {
        while (size() > count)
        {
            pop_back();
        }
        while (size() < count)
        {
            emplace_back(value);
        }
    }

    // swap the contents
    void swap(List& other)
    {
        std::swap(head.node.prev, other.head.node.prev);
        head.node.prev->next = &head.node;
        other.head.node.prev->next = &other.head.node;
        std::swap(head.node.next, other.head.node.next);
        head.node.next->prev = &head.node;
        head.node.next->prev = &head.node;
        other.head.node.next->prev = &other.head.node;
        std::swap(head.node.data, other.head.node.data);
    }

    // merge two sorted lists
    void merge(List& other)
    {
        merge(other, std::less<value_type>());
    }

    void merge(List&& other)
    {
        merge(other, std::less<value_type>());
    }

    template<typename Compare>
    void merge(List& other, Compare comp)
    {
        auto count = other.size();
        auto first1 = begin().node, last1 = end().node, first2 = other.begin().node, last2 = other.end().node;
        node_base_type h, *curr = &h;

        while (first1 != last1 && first2 != last2)
        {
            node_base_type* candidate;
            if (comp(static_cast<node_type*>(first2)->data, static_cast<node_type*>(first1)->data))
            {
                candidate = first2;
                first2 = first2->next;
            }
            else
            {
                candidate = first1;
                first1 = first1->next;
            }
            candidate->prev = curr;
            curr->next = candidate;
            curr = candidate;
        }
        auto first = first1 != last1 ? first1 : first2;
        auto last = first1 != last1 ? last1->prev : last2->prev;
        curr->next = first;
        first->prev = curr;
        last->next = &head.node;
        head.node.prev = last;
        inc_size(count);
        other.head.node.next = other.head.node.prev = &other.head.node;
    }

    template<typename Compare>
    void merge(List&& other, Compare comp)
    {
        merge(other, comp);
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

    void fill_assign(size_type count, const T& value)
    {
        auto first = begin(), last = end();
        for (; first != last && count; ++first, --count)
        {
            *first = value;
        }
        if (count)
        {
            insert(first, count, value);
        }
        else
        {
            erase(first, last);
        }
    }

    template<typename InputIt>
    void range_assign(InputIt first2, InputIt last2)
    {
        auto first1 = begin(), last1 = end();
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            *first1 = *first2;
        }
        if (first2 == last2)
        {
            erase(first1, last1);
        }
        else
        {
            insert(first1, first2, last2);
        }
    }

    template<typename... Args>
    node_base_type* insert_impl(node_base_type* pos, Args&& ... args)
    {
        auto n = create_node(std::forward<Args>(args)...);
        n->hook(pos);
        return n;
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
