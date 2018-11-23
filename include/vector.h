#ifndef VECTOR_H
#define VECTOR_H

#include <type_traits>
#include "allocator.h"
#include "uninitialized.h"
#include "allocator_triats.h"

namespace cyy
{
template<typename T, typename Alloc>
class Vector_base
{
    static_assert(std::is_same<typename Alloc::value_type, T>::value,
                  "Allocator::value_type and T must be the same type.");

public:
    using pointer        = typename allocator_traits<Alloc>::pointer;
    using allocator_type = Alloc;

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
        : data()
    {
    }

    Vector_base(const allocator_type& alloc)
        : data(alloc)
    {
    }

    Vector_base(std::size_t count)
        : data()
    {
        create_storage(count);
    }

    Vector_base(std::size_t count, const allocator_type& alloc)
        : data(alloc)
    {
        create_storage(count);
    }

    Vector_base(Vector_base&& other) noexcept
        : data()
    {
        data.swap(other.data);
    }

    Vector_base(Vector_base&& other, const allocator_type& alloc)
        : data(alloc)
    {
        data.swap(other.data);
    }

    ~Vector_base()
    {
        deallocate(data.start, data.end_of_storage - data.start);
    }

    pointer allocate(std::size_t n)
    {
        return n != 0 ? allocator_traits<Alloc>::allocate(static_cast<allocator_type&>(data), n) : pointer();
    }

    void deallocate(pointer p, std::size_t n)
    {
        if (p)
            allocator_traits<Alloc>::deallocate(static_cast<allocator_type&>(data), p, n);
    }

    allocator_type& get_alloc_ref() noexcept
    {
        return static_cast<allocator_type&>(data);
    }

    const allocator_type& get_alloc_ref() const noexcept
    {
        return static_cast<const allocator_type&>(data);
    }

    allocator_type get_allocator() const noexcept
    {
        return allocator_type();
    }

    Vector_data data;

private:
    void create_storage(std::size_t count)
    {
        data.start = allocate(count);
        data.finish = data.start;
        data.end_of_storage = data.start + count;
    }
};

template<typename T, typename Alloc = cyy::allocator<T>>
class Vector
    : public Vector_base<T, Alloc>
{
    using Base = Vector_base<T, Alloc>;
    using Base::allocate;
    using Base::deallocate;
    using Base::data;
    using Base::get_alloc_ref;

public:
    using value_type             = T;
    using allocator_type         = Alloc;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename cyy::allocator_traits<Alloc>::pointer;
    using const_pointer          = typename cyy::allocator_traits<Alloc>::const_pointer;
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

    explicit Vector(size_type count, const T& value, const allocator_type& alloc = Alloc())
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

    template<typename InputIt>
    Vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : Base(std::distance(first, last), alloc)
    {
        range_initialize(first, last, typename std::iterator_traits<InputIt>::iterator_category());
    }

    // destruct
    ~Vector()
    {
        // TODO: optimize for POD
        destroy();
    }

    // assignment
    Vector& operator=(const Vector& rhs)
    {
        // TODO: handle exception and optimize time
        size_type size = rhs.size();
        if (allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value)
        {
            get_alloc_ref() = rhs.get_alloc_ref();
        }

        // destruct and deallocate *this
        destroy();
        deallocate(data.start, data.end_of_storage - data.start);
        data.start = data.finish = data.end_of_storage = nullptr;
        data.start = allocate(size);
        copy_initialize(rhs);
        return *this;
    }

    Vector& operator=(Vector&& rhs)
    {
        if (allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value)
        {
            get_alloc_ref() = std::move(rhs.get_alloc_ref());
        }
        else if (get_alloc_ref() != rhs.get_alloc_ref())
        {
            move_initialize(std::move(rhs));
            return *this;
        }
        Vector tmp{allocator_type()};
        data.swap(tmp.data);
        data.swap(rhs.data);

        return *this;
    }

    Vector& operator=(std::initializer_list<value_type> l)
    {
        Vector tmp{allocator_type()};
        data.swap(tmp.data);
        range_initialize(l.begin(), l.end(), std::forward_iterator_tag());
        return *this;
    }

    // get the number of elements 
    size_type size() const noexcept
    {
        return data.finish - data.start;
    }

    // 
    iterator begin() noexcept
    {
        return iterator(data.start);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(data.start);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(data.start);
    }

    iterator end() noexcept
    {
        return iterator(data.finish);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(data.finish);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(data.finish);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(data.finish-1);
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(data.finish-1);
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(data.finish-1);
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(data.first-1);
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(data.start-1);
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(data.start-1);
    }

private:
    void default_initialize(size_type count)
    {
        data.finish = cyy::uninitialized_default_n_a(data.start, count, get_alloc_ref());
    }

    void fill_initialize(size_type count, const value_type& value)
    {
        data.finish = cyy::uninitialized_fill_n_a(data.start, count, value, get_alloc_ref());
    }

    void copy_initialize(const Vector& other)
    {
        data.finish = cyy::uninitialized_copy_a(other.begin(), other.end(),
                                                data.start, get_alloc_ref());
    }

    void move_initialize(Vector&& other)
    {
        data.finish = cyy::uninitialized_move_a(other.begin(), other.end(), data.start, get_alloc_ref());
    }

    template<typename InputIt>
    void range_initialize(InputIt first, InputIt last, std::input_iterator_tag)
    {
        for (; first != last;)
        {
            emplace_back(*first);
        }
    }

    template<typename ForwardIt>
    void range_initialize(ForwardIt first, ForwardIt last, std::forward_iterator_tag)
    {
        data.finish = cyy::uninitialized_copy_a(first, last, data.start, get_alloc_ref());
    }

    void destroy()
    {
        for (auto cur = data.start; cur != data.finish; ++cur)
        {
            allocator_traits<allocator_type>::destroy(get_alloc_ref(), std::addressof(*cur));
        }
    }

};

} // namespace cyy
#endif // !VECTOR_H
