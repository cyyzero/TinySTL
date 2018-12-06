#ifndef VECTOR_H
#define VECTOR_H

#include <limits>
#include <algorithm>
#include <type_traits>
#include "allocator.h"
#include "uninitialized.h"
#include "allocator_traits.h"
#include "construct.h"

namespace cyy
{

// Base class for Vector, used for allocating
template<typename T, typename Alloc>
class Vector_base
{
    static_assert(std::is_same<typename Alloc::value_type, T>::value,
                  "Allocator::value_type and T must be the same type.");

public:
    using allocator_type   = Alloc;
    using allocator_traits = cyy::allocator_traits<Alloc>;
    using pointer          = typename allocator_traits::pointer;

protected:
    struct Vector_data
        : public Alloc
    {
        Vector_data()
            : Alloc(), start(), finish(), end_of_storage()
        {
        }

        Vector_data(const allocator_type& alloc)
            : Alloc(alloc), start(), finish(), end_of_storage()
        {
        }

        Vector_data(Alloc&& alloc)
            : Alloc(std::move(alloc)), start(), finish(), end_of_storage()
        {
        }

        void swap(Vector_data& other) noexcept
        {
            // using namespace std;
            std::swap(start, other.start);
            std::swap(finish, other.finish);
            std::swap(end_of_storage, other.end_of_storage);
        }

        pointer start;
        pointer finish;
        pointer end_of_storage;
    };

    Vector_base()
        : data_impl()
    {
    }

    Vector_base(const allocator_type& alloc)
        : data_impl(alloc)
    {
    }

    Vector_base(std::size_t count)
        : data_impl()
    {
        create_storage(count);
    }

    Vector_base(std::size_t count, const allocator_type& alloc)
        : data_impl(alloc)
    {
        create_storage(count);
    }

    Vector_base(Vector_base&& other) noexcept
        : data_impl()
    {
        data_impl.swap(other.data_impl);
    }

    Vector_base(Vector_base&& other, const allocator_type& alloc)
        : data_impl(alloc)
    {
        data_impl.swap(other.data_impl);
    }

    ~Vector_base()
    {
        deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
    }

    pointer allocate(std::size_t n)
    {
        return n != 0 ? allocator_traits::allocate(static_cast<allocator_type&>(data_impl), n) : pointer();
    }

    void deallocate(pointer p, std::size_t n)
    {
        if (p)
            allocator_traits::deallocate(static_cast<allocator_type&>(data_impl), p, n);
    }

    allocator_type& get_alloc_ref() noexcept
    {
        return static_cast<allocator_type&>(data_impl);
    }

    const allocator_type& get_alloc_ref() const noexcept
    {
        return static_cast<const allocator_type&>(data_impl);
    }

    allocator_type get_allocator() const noexcept
    {
        return allocator_type();
    }

    Vector_data data_impl;

private:
    void create_storage(std::size_t count)
    {
        data_impl.start = allocate(count);
        data_impl.finish = data_impl.start;
        data_impl.end_of_storage = data_impl.start + count;
    }
};

template<typename T, typename Alloc = cyy::allocator<T>>
class Vector
    : public Vector_base<T, Alloc>
{
    using Base = Vector_base<T, Alloc>;
    using Base::allocate;
    using Base::deallocate;
    using Base::data_impl;
    using Base::get_alloc_ref;
    using allocator_traits = typename Base::allocator_traits;


public:
    using value_type             = T;
    using allocator_type         = Alloc;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename allocator_traits::pointer;
    using const_pointer          = typename allocator_traits::const_pointer;
    using iterator               = T*;
    using const_iterator         = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // construct
    Vector()
        : Base()
    {
    }

    explicit Vector(const allocator_type& alloc)
        : Base(alloc)
    {
    }

    explicit Vector(size_type count, const value_type& value, const allocator_type& alloc = Alloc())
        : Base(count, alloc)
    {
        fill_initialize(count, value);
    }

    explicit Vector(size_type count, const allocator_type& alloc = Alloc())
        : Base(count, alloc)
    {
        default_initialize(count);
    }

    Vector(const Vector& other)
        : Base(other.size(), other.get_allocator())
    {
        copy_initialize(other);
    }

    Vector(const Vector& other, const allocator_type& alloc)
        : Base(other.size(), alloc)
    {
        copy_initialize(other);
    }

    Vector(Vector&& other) noexcept
        : Base(std::move(other))
    {
    }

    Vector(Vector&& other, const allocator_type& alloc)
        : Base(std::move(other), alloc)
    {
        if (alloc != other.get_allocator())
        {
            move_initialize(std::move(other));
            other.clear();
        }
    }

    Vector(std::initializer_list<value_type> l, const allocator_type& alloc = allocator_type())
        : Base(l.size() ,alloc)
    {
        range_initialize(l.begin(), l.end(), std::forward_iterator_tag());
    }

    template<typename InputIterator, typename = std::enable_if_t<!std::is_integral_v<InputIterator>>>
    Vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
        : Base(std::distance(first, last), alloc)
    {
        range_initialize(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }

    // destruct
    ~Vector()
    {
        erase_at_end(data_impl.start);
    }

    // assignment
    Vector& operator=(const Vector& rhs)
    {
        // TODO: handle exception and optimize time
        // size_type size = rhs.size();
        if (allocator_traits::propagate_on_container_copy_assignment::value)
        {
            get_alloc_ref() = rhs.get_alloc_ref();
        }

        // destruct and deallocate *this
        Vector tmp(rhs);
        data_impl.swap(tmp.data_impl);
        return *this;
    }

    Vector& operator=(Vector&& rhs)
    {
        if (allocator_traits::propagate_on_container_copy_assignment::value)
        {
            get_alloc_ref() = std::move(rhs.get_alloc_ref());
        }
        else if (get_alloc_ref() != rhs.get_alloc_ref())
        {
            assign(rhs.begin(), rhs.end());
            return *this;
        }

        Vector tmp{allocator_type()};
        data_impl.swap(tmp.data_impl);
        data_impl.swap(rhs.data_impl);

        return *this;
    }

    Vector& operator=(std::initializer_list<value_type> l)
    {
        assign(l.begin(), l.end());
        return *this;
    }

    // replace the contents
    void assign(size_type count, const value_type& value)
    {
        if (count > capacity())
        {
            Vector tmp(count, value);
            data_impl.swap(tmp.data_impl);
        }
        else if (count > size())
        {
            std::fill(data_impl.start, data_impl.finish, value);
            data_impl.finish = cyy::uninitialized_fill_n_a(end(), count-size(), value, get_alloc_ref());
        }
        else
        {
            erase_at_end(std::fill_n(data_impl.start, count, value));
        }
    }

    template<typename InputIt>
    void assign(InputIt first, InputIt last)
    {
        using Iterator_category = typename std::iterator_traits<InputIt>::iterator_category;
        assign_aux(first, last, Iterator_category());
    }

    void assign(std::initializer_list<value_type> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    // access specified element
    reference& operator[](size_type index)
    {
        return *(data_impl.start + index);
    }

    const_reference operator[](size_type index) const
    {
        return *(data_impl.start + index);
    }

    // access specified element with bounds checking 
    reference at(size_type index)
    {
        if (index >= size())
            throw std::out_of_range("index is out of range");

        return *(data_impl.start + index);
    }

    const_reference at(size_type index) const
    {
        if (index >= size())
            throw std::out_of_range("index is out of range");

        return *(data_impl.start + index);
    }

    // access the first element 
    reference front()
    {
        if (empty())
            throw std::logic_error("Empty vector has no front");

        return *data_impl.start;
    }

    const_reference front() const
    {
        if (empty())
            throw std::logic_error("Empty vector has no front");

        return *data_impl.start;
    }

    // access the last element 
    reference back()
    {
        if (empty())
            throw std::logic_error("Empry vector has no back");

        return *(data_impl.finish - 1);
    }

    const_reference back() const
    {
        if (empty())
            throw std::logic_error("Empry vector has no back");

        return *(data_impl.finish - 1);
    }

    // direct access to the underlying array 
    pointer data() noexcept
    {
        return data_impl.start;
    }

    const_pointer data() const noexcept
    {
        return data_impl.start;
    }

    // get iterators
    iterator begin() noexcept
    {
        return iterator(data_impl.start);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(data_impl.start);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(data_impl.start);
    }

    iterator end() noexcept
    {
        return iterator(data_impl.finish);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(data_impl.finish);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(data_impl.finish);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(data_impl.finish-1);
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(data_impl.finish-1);
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(data_impl.finish-1);
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(data_impl.start-1);
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(data_impl.start-1);
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(data_impl.start-1);
    }

    // checks whether the container is empty
    bool empty() const noexcept
    {
        return data_impl.start == data_impl.finish;
    }

    // returns the number of elements
    size_type size() const noexcept
    {
        return data_impl.finish - data_impl.start;
    }

    // returns the maximum possible number of elements
    size_type max_size() const noexcept
    {
        return std::numeric_limits<size_type>::max();
    }

    // returns the number of elements that can be held in currently allocated storage
    size_type capacity() const noexcept
    {
        return data_impl.end_of_storage - data_impl.start;
    }

    // reserves storage 
    void reserve(size_type new_cap)
    {
        if (new_cap > max_size())
            throw std::length_error("too large");

        if (new_cap <= capacity())
            return;

        Vector tmp(new_cap);
        tmp.move_initialize(std::move(*this));
        data_impl.swap(tmp.data_impl);
    }

    // reduces memory usage by freeing unused memory
    void shrink_to_fit()
    {
        if (capacity() > 2 * size())
        {
            Vector tmp(size());
            tmp.move_initialize(std::move(*this));
            data_impl.swap(tmp.data_impl);
        }
    }


    // erase all elements
    void clear() noexcept
    {
        erase_at_end(data_impl.start);
    }

    iterator insert(const_iterator pos, const value_type& value)
    {
        return insert_at_pos(pos, value);
    }

    iterator insert(const_iterator pos, value_type&& value)
    {
        return insert_at_pos(pos, std::move(value));
    }

    iterator insert(const_iterator pos, size_type count, const value_type& value)
    {
        size_type dist = pos - data_impl.start;
        insert_n(data_impl.start + dist, count, value);
        return begin() + dist;
    }

    template<typename InputIterator, typename = std::enable_if_t<!std::is_integral_v<InputIterator>>>
    iterator insert(const_iterator pos, InputIterator first, InputIterator last)
    {
        size_type dist = pos - data_impl.start;
        insert_range(data_impl.start + dist, first, last);
        return begin() + dist;
    }

    iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    // removes the element at pos
    iterator erase(iterator pos)
    {
        return erase_at_pos(pos+1, end(), pos);
    }

    // removes the elements in the range [first, last)
    iterator erase(iterator first, iterator last)
    {
        return erase_at_pos(last, end(), first);
    }

    // Appends the given element value to the end of the container

    /// The new element is initialized as a copy of value.
    void push_back(const T& value)
    {
        if (data_impl.finish == data_impl.end_of_storage)
        {
            expand();
        }
        allocator_traits::construct(get_alloc_ref(), data_impl.finish++, value);
    }

    /// value is moved into the new element.
    void push_back(T&& value)
    {
        if (data_impl.finish == data_impl.end_of_storage)
        {
            expand();
        }
        allocator_traits::construct(get_alloc_ref(), data_impl.finish++, std::move(value));
    }

    // removes the last element
    void pop_back()
    {
        allocator_traits::destroy(get_alloc_ref(), data_impl.finish--);
    }

    void resize(size_type count)
    {
        if (count > capacity())
        {
            size_type orignal_size = size();
            pointer start = allocate_and_copy(count, begin(), end());
            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = start + orignal_size;
            data_impl.end_of_storage = start + count;
            for (size_type i = 0; i < count - orignal_size; ++i, ++data_impl.finish)
            {
                allocator_traits::construct(get_alloc_ref(), data_impl.finish);
            }
        }
        else if (count > size())
        {
            size_type append_size = count - size();
            for (size_type i = 0; i < append_size; ++i, ++data_impl.finish)
            {
                allocator_traits::construct(get_alloc_ref(), data_impl.finish);
            }
        }
        else
        {
            erase_at_end(data_impl.start + count);
        }
    }

    void resize(size_type count, const value_type& value)
    {
        if (count > capacity())
        {
            size_type orignal_size = size();
            pointer start = allocate_and_copy(count, begin(), end());
            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = start + orignal_size;
            data_impl.end_of_storage = start + count;
            for (size_type i = 0; i < count - orignal_size; ++i, ++data_impl.finish)
            {
                allocator_traits::construct(get_alloc_ref(), data_impl.finish, value);
            }
        }
        else if (count > size())
        {
            size_type append_size = count - size();
            for (size_type i = 0; i < append_size; ++i, ++data_impl.finish)
            {
                allocator_traits::construct(get_alloc_ref(), data_impl.finish, value);
            }
        }
        else
        {
            erase_at_end(data_impl.start + count);
        }
    }

    // exchange contents
    void swap(Vector& other)
    {
        if (allocator_traits::propagate_on_container_swap::value)
            std::swap(get_alloc_ref(), other.get_alloc_ref());
        data_impl.swap(other.data_impl);
    }

private:
    void default_initialize(size_type count)
    {
        data_impl.finish = cyy::uninitialized_default_n_a(data_impl.start, count, get_alloc_ref());
    }

    void fill_initialize(size_type count, const value_type& value)
    {
        data_impl.finish = cyy::uninitialized_fill_n_a(data_impl.start, count, value, get_alloc_ref());
    }

    void copy_initialize(const Vector& other)
    {
        data_impl.finish = cyy::uninitialized_copy_a(other.begin(), other.end(),
                                                data_impl.start, get_alloc_ref());
    }

    void move_initialize(Vector&& other)
    {
        data_impl.finish = cyy::uninitialized_move_a(other.begin(), other.end(), data_impl.start, get_alloc_ref());
    }

    template<typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last, std::input_iterator_tag)
    {
        for (; first != last;)
        {
            emplace_back(*first);
        }
    }

    template<typename ForwardIt>
    void range_initialize(ForwardIt first, ForwardIt last, std::forward_iterator_tag)
    {
        data_impl.finish = cyy::uninitialized_copy_a(first, last, data_impl.start, get_alloc_ref());
    }

    // allocate memory and copy elements into it
    template<typename ForwardIterator>
    pointer allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last)
    {
        pointer start = allocate(n);
        try 
        {
            cyy::uninitialized_copy_a(first, last, start, get_alloc_ref());
            return start;
        }
        catch(...)
        {
            deallocate(start, n);
            throw;
        }
    }

    // destroy elements in range of [pos, finish)
    void erase_at_end(pointer pos) noexcept
    {
        cyy::Destroy(pos, data_impl.finish, get_alloc_ref());
        data_impl.finish = pos;
    }

    template<typename InputIterator>
    void assign_aux(InputIterator first, InputIterator last, std::input_iterator_tag)
    {
        pointer cur = data_impl.start;
        for (; first != last && cur != data_impl.finish; ++cur, ++first)
        {
            *cur = *first;
        }
        if (first == last)
        {
            erase_at_end(cur);
        }
        else
        {
            insert(end(), first, last);
        }
    }

    // do real assign work
    template<typename ForwardIterator>
    void assign_aux(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag)
    {
        const size_type n = std::distance(first, last);
        if (n > capacity())
        {
            pointer start = allocate_and_copy(n, first, last);
            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = data_impl.end_of_storage = start + n;
        }
        else if (n > size())
        {
            ForwardIterator mid = first;
            std::advance(mid, n);
            std::copy(first, mid, data_impl.start);
            data_impl.finish = cyy::uninitialized_copy_a(mid, last, data_impl.finish, get_alloc_ref());
        }
        else
        {
            // there's a better version
            erase_at_end(std::copy(first, last, data_impl.start));
        }
    }

    iterator erase_at_pos(iterator first, iterator last, iterator target)
    {
        iterator dest = target;
        for (; first != last; ++first, ++target)
        {
            *target = std::move(*first);
        }
        pointer p = std::addressof(*target);
        erase_at_end(p);
        return dest;
    }

    void expand()
    {
        size_type n = capacity();
        size_type alloc_n = (n == 0) ? 1 : n*2;
        pointer start = allocate(alloc_n);

        try
        {
            cyy::uninitialized_move_a(begin(), end(), start, get_alloc_ref());
        }
        catch (...)
        {
            deallocate(start, alloc_n);
            throw;
        }

        cyy::Destroy(begin(), end());
        deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
        data_impl.start = start;
        data_impl.finish = start+n;
        data_impl.end_of_storage = start + alloc_n;
    }

    template<typename Value>
    iterator insert_at_pos(const_iterator pos, Value&& value)
    {
        // equal to size() == capacity()
        if (data_impl.end_of_storage == data_impl.finish)
        {
            size_type dist = std::distance(cbegin(), pos);
            size_type orignal_size = size();
            pointer start = allocate(2*orignal_size);
            try
            {
                cyy::uninitialized_move_a(data_impl.start, data_impl.start + dist, start, get_alloc_ref());
                allocator_traits::construct(get_alloc_ref(), start+dist, std::forward<Value>(value));
                cyy::uninitialized_move_a(data_impl.start+dist, data_impl.finish, start+dist+1, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, 2*orignal_size);
                throw;
            }
            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = start + orignal_size + 1;
            data_impl.end_of_storage = start + 2 * orignal_size;
            return start + dist;
        }
        else
        {
            allocator_traits::construct(get_alloc_ref(), data_impl.finish, std::move(*(data_impl.finish-1)));
            ++data_impl.finish;
            iterator cur;
            for (cur = end()-2; cur != pos; --cur)
            {
                *cur = std::move(*(cur-1));
            }
            *cur = std::forward<Value>(value);
            return cur;
        }
    }

    void insert_n(pointer pos, size_type count, const value_type& value)
    {
        pointer insert_end = pos + count;

        if (data_impl.finish + count > data_impl.end_of_storage)
        {
            size_type orignal_capacity;
            pointer cur, start;

            orignal_capacity = capacity();

            start = allocate(orignal_capacity + count);
            try
            {
                cur = cyy::uninitialized_move_a(data_impl.start, pos, start, get_alloc_ref());
                cur = cyy::uninitialized_fill_n_a(cur, count, value, get_alloc_ref());
                cyy::uninitialized_move_a(pos, data_impl.finish, cur, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, orignal_capacity + count);
                throw;
            }

            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = cur;
            data_impl.end_of_storage = cur;
        }
        else
        {
            if (insert_end > data_impl.finish)
            {
                cyy::uninitialized_move_a(pos, data_impl.finish, insert_end, get_alloc_ref());
                std::fill_n(pos, data_impl.finish - pos, value);
                cyy::uninitialized_fill_a(data_impl.finish, insert_end, value, get_alloc_ref());
            }
            else
            {
                cyy::uninitialized_move_a(data_impl.finish-count, data_impl.finish,
                                          data_impl.finish, get_alloc_ref());
                std::move(pos, data_impl.finish - count, insert_end);
                std::fill_n(pos, count, value);
            }
            data_impl.finish = data_impl.finish + count;
        }
    }

    template<typename InputIterator>
    void insert_range(pointer pos, InputIterator first, InputIterator last)
    {
        const size_type count = std::distance(first, last);
        pointer insert_end = pos + count;
        // need to reallocate
        if (data_impl.finish + count > data_impl.end_of_storage)
        {
            pointer start, cur;
            size_type orignal_capacity = capacity();
            start = allocate(orignal_capacity + count);
            printf("%p alloc\n", (void*)this);
            try
            {
                cur = cyy::uninitialized_move_a(data_impl.start, pos, start, get_alloc_ref());
                cur = cyy::uninitialized_copy_a(first, last, cur, get_alloc_ref());
                cur = cyy::uninitialized_move_a(pos, data_impl.finish, cur, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, orignal_capacity + count);
            }

            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = cur;
            data_impl.end_of_storage = cur;
        }
        else
        {
            if (insert_end > data_impl.finish)
            {
                size_type pos_to_finish = data_impl.finish - pos;
                auto mid = first + pos_to_finish;
                cyy::uninitialized_move_a(pos, data_impl.finish, insert_end, get_alloc_ref());
                std::copy(first, mid, pos);
                cyy::uninitialized_copy_a(mid, last, data_impl.finish, get_alloc_ref());
            }
            else
            {
                std::cout << size() << " "<< capacity() << " " << count << "\n";
                cyy::uninitialized_move_a(data_impl.finish-count, data_impl.finish, data_impl.finish, get_alloc_ref());
                std::move(pos, data_impl.finish-count, insert_end);
                std::copy(first, last, pos);
            }
            data_impl.finish = data_impl.finish + count;
        }
    }
};

} // namespace cyy
#endif // !VECTOR_H
