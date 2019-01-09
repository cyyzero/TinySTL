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

namespace detail
{
// Base class for Vector, used for allocating
template<typename T, typename Alloc>
class Vector_base
{
    static_assert(std::is_same<typename Alloc::value_type, T>::value,
                  "Allocator::value_type and T must be the same type.");

public:
    using Alloc_type       = Alloc;
    using Alloc_traits     = cyy::Allocator_traits<Alloc>;
    using pointer          = typename Alloc_traits::pointer;

protected:
    struct Vector_data
        : public Alloc
    {
        Vector_data()
            : Alloc(), start(), finish(), end_of_storage()
        {
        }

        Vector_data(const Alloc_type& alloc)
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

    Vector_base(const Alloc_type& alloc)
        : data_impl(alloc)
    {
    }

    Vector_base(std::size_t count)
        : data_impl()
    {
        create_storage(count);
    }

    Vector_base(std::size_t count, const Alloc_type& alloc)
        : data_impl(alloc)
    {
        create_storage(count);
    }

    Vector_base(Vector_base&& other) noexcept
        : data_impl()
    {
        data_impl.swap(other.data_impl);
    }

    Vector_base(Vector_base&& other, const Alloc_type& alloc)
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
        return n != 0 ? Alloc_traits::allocate(static_cast<Alloc_type&>(data_impl), n) : pointer();
    }

    void deallocate(pointer p, std::size_t n)
    {
        if (p)
            Alloc_traits::deallocate(static_cast<Alloc_type&>(data_impl), p, n);
    }

    Alloc_type& get_alloc_ref() noexcept
    {
        return static_cast<Alloc_type&>(data_impl);
    }

    const Alloc_type& get_alloc_ref() const noexcept
    {
        return static_cast<const Alloc_type&>(data_impl);
    }

    Alloc_type get_allocator() const noexcept
    {
        return Alloc_type();
    }

    Vector_data data_impl;

private:
    void create_storage(std::size_t count)
    {
        data_impl.start = allocate(count);
        data_impl.finish = data_impl.start;
        data_impl.end_of_storage = data_impl.start + count;
    }

}; // class Vector_base
} // namespace detail

template<typename T, typename Alloc = cyy::Allocator<T>>
class Vector
    : public detail::Vector_base<T, Alloc>
{
    using Base = detail::Vector_base<T, Alloc>;
    using Base::allocate;
    using Base::deallocate;
    using Base::data_impl;
    using Base::get_alloc_ref;
    using Alloc_traits = typename Base::Alloc_traits;

public:
    using value_type             = T;
    using allocator_type         = Alloc;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename Alloc_traits::pointer;
    using const_pointer          = typename Alloc_traits::const_pointer;
    using iterator               = typename Alloc_traits::pointer;
    using const_iterator         = typename Alloc_traits::const_pointer;
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
        using Iterator_category =  typename std::iterator_traits<InputIterator>::iterator_category;
        range_initialize(first, last, Iterator_category());
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
        if (Alloc_traits::propagate_on_container_copy_assignment::value)
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
        if (Alloc_traits::propagate_on_container_copy_assignment::value)
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

    Vector& operator=(std::initializer_list<value_type> ilist)
    {
        range_assign(ilist.begin(), ilist.end(), std::forward_iterator_tag());
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

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        using Iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
        range_assign(first, last, Iterator_category());
    }

    void assign(std::initializer_list<value_type> ilist)
    {
        range_assign(ilist.begin(), ilist.end(), std::forward_iterator_tag());
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
    T* data() noexcept
    {
        return data_ptr(data_impl.first);
    }

    const T* data() const noexcept
    {
        return data_ptr(data_impl.first);
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

    // insert lvalue before pos
    iterator insert(const_iterator pos, const value_type& value)
    {
        return insert_at_pos(pos, value);
    }

    // insert rvalue before pos
    iterator insert(const_iterator pos, value_type&& value)
    {
        return insert_at_pos(pos, std::move(value));
    }

    // inserts count copies of the value before pos
    iterator insert(const_iterator pos, size_type count, const value_type& value)
    {
        size_type dist = pos - data_impl.start;
        fill_insert(data_impl.start + dist, count, value);
        return begin() + dist;
    }

    // inserts elements from range [first, last) before pos
    template<typename InputIterator, typename = std::enable_if_t<!std::is_integral_v<InputIterator>>>
    iterator insert(const_iterator pos, InputIterator first, InputIterator last)
    {
        size_type dist = pos - data_impl.start;
        using Iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
        range_insert(data_impl.start + dist, first, last, Iterator_category());
        return begin() + dist;
    }

    // inserts elements from initializer list ilist before pos
    iterator insert(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    // constructs element in-place
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        return insert_at_pos(pos, std::forward<Args>(args)...);
    }

    // constructs an element in-place at the end
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        insert_at_pos(cend(), std::forward<Args>(args)...);
    }

    // removes the element at pos
    iterator erase(const_iterator pos)
    {
        pointer p = pos - cbegin() + data_impl.start;
        return range_erase(p, p+1);
    }

    // removes the elements in the range [first, last)
    iterator erase(const_iterator first, iterator last)
    {
        pointer first_p = first - cbegin() + data_impl.start;
        pointer last_p  = last - first + first_p;
        return range_erase(first_p, last_p);
    }

    // Appends the given element value to the end of the container

    /// The new element is initialized as a copy of value.
    void push_back(const T& value)
    {
        if (data_impl.finish == data_impl.end_of_storage)
        {
            expand();
        }
        Alloc_traits::construct(get_alloc_ref(), data_impl.finish++, value);
    }

    /// value is moved into the new element.
    void push_back(T&& value)
    {
        if (data_impl.finish == data_impl.end_of_storage)
        {
            expand();
        }
        Alloc_traits::construct(get_alloc_ref(), data_impl.finish++, std::move(value));
    }

    // removes the last element
    void pop_back()
    {
        Alloc_traits::destroy(get_alloc_ref(), data_impl.finish--);
    }

    // changes the number of elements stored
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
            cyy::uninitialized_default_n_a(data_impl.finish, count - orignal_size, get_alloc_ref());
        }
        else if (count > size())
        {
            size_type append_size = count - size();
            cyy::uninitialized_default_n_a(data_impl.finish, append_size, get_alloc_ref());
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
            cyy::uninitialized_fill_n_a(data_impl.finish, count - orignal_size, value, get_alloc_ref());
        }
        else if (count > size())
        {
            size_type append_size = count - size();
            cyy::uninitialized_fill_n_a(data_impl.finish, append_size, value, get_alloc_ref());
        }
        else
        {
            erase_at_end(data_impl.start + count);
        }
    }

    // exchange contents
    void swap(Vector& other)
    {
        if (Alloc_traits::propagate_on_container_swap::value)
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

    // 
    template<typename U>
    U* data_ptr(U* ptr) const
    {
        return ptr;
    }

    template<typename U>
    typename cyy::pointer_traits<U>::element_type*
    data_ptr(U ptr) const
    {
        return empty() ? nullptr : std::addressof(*ptr);
    }

    // check alloc size
    size_type check_length(size_type n, const char *s) const
    {
        if (max_size() - size() < n)
            throw std::length_error(s);

        const size_type len = size() + std::max(size(), n);
        return (len < size() || len > max_size()) ? max_size() : len;
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

    // do real assign work
    template<typename InputIterator>
    void range_assign(InputIterator first, InputIterator last, std::input_iterator_tag)
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

    template<typename ForwardIterator>
    void range_assign(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag)
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
            erase_at_end(std::copy(first, last, data_impl.start));
        }
    }

    iterator range_erase(pointer first, pointer last)
    {
        pointer finish = std::move(last, data_impl.finish, first);
        erase_at_end(finish);
        return iterator(first);
    }

    void expand()
    {
        size_type n = capacity();
        size_type alloc_n = check_length(1, "Vector::expand");
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

        cyy::Destroy(data_impl.start, data_impl.finish);
        deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
        data_impl.start = start;
        data_impl.finish = start+n;
        data_impl.end_of_storage = start + alloc_n;
    }

    template<typename... Args>
    iterator insert_at_pos(const_iterator pos, Args&&... args)
    {
        // equal to size() == capacity()
        if (data_impl.end_of_storage == data_impl.finish)
        {
            size_type dist = std::distance(cbegin(), pos);
            size_type orignal_size = size();
            size_type alloc_size = check_length(1, "Vector::insert_at_pos");
            pointer start = allocate(alloc_size);
            try
            {
                cyy::uninitialized_move_a(data_impl.start, data_impl.start + dist, start, get_alloc_ref());
                Alloc_traits::construct(get_alloc_ref(), start+dist, std::forward<Args>(args)...);
                cyy::uninitialized_move_a(data_impl.start+dist, data_impl.finish, start+dist+1, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, alloc_size);
                throw;
            }
            erase_at_end(data_impl.start);
            deallocate(data_impl.start, data_impl.end_of_storage - data_impl.start);
            data_impl.start = start;
            data_impl.finish = start + orignal_size + 1;
            data_impl.end_of_storage = start + alloc_size;
            return start + dist;
        }
        else
        {
            Alloc_traits::construct(get_alloc_ref(), data_impl.finish, std::move(*(data_impl.finish-1)));
            ++data_impl.finish;
            pointer target = data_impl.start + (pos - cbegin());
            std::move_backward(target, data_impl.finish-1, data_impl.finish);
            *target = value_type(std::forward<Args>(args)...);
            return iterator(target);
        }
    }

    void fill_insert(pointer pos, size_type count, const value_type& value)
    {
        pointer insert_end = pos + count;

        if (count > data_impl.end_of_storage - data_impl.finish)
        {
            size_type alloc_size;
            pointer cur, start;

            alloc_size = check_length(count, "Vector::fill_insert");

            start = allocate(alloc_size);
            try
            {
                cur = cyy::uninitialized_move_a(data_impl.start, pos, start, get_alloc_ref());
                cur = cyy::uninitialized_fill_n_a(cur, count, value, get_alloc_ref());
                cyy::uninitialized_move_a(pos, data_impl.finish, cur, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, alloc_size);
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
    void range_insert(pointer pos, InputIterator first, InputIterator last, std::input_iterator_tag)
    {
        for (; first != last; ++first)
        {
            insert_at_pos(pos, *first);
            pos += 2;
        }
    }

    template<typename ForwardIterator>
    void range_insert(pointer pos, ForwardIterator first, ForwardIterator last, std::forward_iterator_tag)
    {
        const size_type count = std::distance(first, last);
        pointer insert_end = pos + count;
        // need to reallocate
        if (count > data_impl.end_of_storage - data_impl.finish)
        {
            pointer start, cur;
            size_type alloc_size = check_length(count, "Vector::range_insert");
            start = allocate(alloc_size);
            try
            {
                cur = cyy::uninitialized_move_a(data_impl.start, pos, start, get_alloc_ref());
                cur = cyy::uninitialized_copy_a(first, last, cur, get_alloc_ref());
                cur = cyy::uninitialized_move_a(pos, data_impl.finish, cur, get_alloc_ref());
            }
            catch (...)
            {
                deallocate(start, alloc_size);
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
                cyy::uninitialized_move_a(data_impl.finish-count, data_impl.finish, data_impl.finish, get_alloc_ref());
                std::move(pos, data_impl.finish-count, insert_end);
                std::copy(first, last, pos);
            }
            data_impl.finish = data_impl.finish + count;
        }
    }
}; // class Vector

// lexicographically compares the values in the vector 
/// return true if all elements are equal, false otherwise
template<typename T, typename Allocator>
inline bool operator==(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/// base on operator==
template<typename T, typename Allocator>
inline bool operator!=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

/// return true if all eleemnts of the lhs are lexicographically less than those of rhs, false otherwise
template<typename T, typename Allocator>
inline bool operator<(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/// base on operator<
template<typename T, typename Allocator>
inline bool operator>(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return rhs < lhs;
}

/// base on operator<
template<typename T, typename Allocator>
inline bool operator<=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return !(rhs < lhs);
}

/// base on operator<
template<typename T, typename Allocator>
inline bool operator>=(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs)
{
    return !(lhs < rhs);
}

// swap Vector
template<typename T, typename Allocator>
inline void swap(Vector<T, Allocator>& x, Vector<T, Allocator>& y)
{
    x.swap(y);
}

} // namespace cyy
#endif // !VECTOR_H
