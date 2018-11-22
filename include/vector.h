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

    struct Vector_data
        : public Alloc
    {
        Vector_data()
            : Alloc(), start(), finish(), end_of_storage()
        {
        }

        Vector_data(const allocator_type& alloc)
            : Alloc(alloc), start(), finish(), end_of_storate()
        {
        }

        Vector_data(Alloc&& alloc)
            : Alloc(std::move(alloc)), start(), finish(), end_of_storage()
        {
        }

        void swap(Vector_data& other) noexcept
        {
            using namespace std;
            swap(start, other.start);
            swap(finish, other.finish);
            swap(end_of_storage, other.end_of_storage);
        }

        pointer start;
        pointer finish;
        pointer end_of_storage;
    };

public:
    using pointer        = typename allocator_traits<Alloc>::pointer;
    using allocator_type = Alloc;

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
        create_storate(count);
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

    pointer allocate(std::size_t n)
    {
        return n != 0 ? allocator_traits<Alloc>::allocate(static_cast<Alloc&>(data), n) : pointer();
    }

    void deallocate(pointer p, std::size_t n);
    {
        if (p)
            allocator_traits<Alloc>::deallocate(static_cast<Alloc&>(data), p, n);
    }

    allocator_type get_allocator() const noexcept
    {
        return Alloc();
    }

    Vector_data data;
private:
    void create_storage(size_t count)
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

public:
    using value_type             = T;
    using allocator_type         = Allocator;
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

    Vector()
        : Vector_base()
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
        copy_initialize(other)
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
            copy_initialize(other);
            other.clear();
        }
    }

    Vector(std::initializer_list<value_type> l, const allocator_type& alloc = allocator_type())
        : Base(alloc)
    {
        range_initialize(l.begin(), l.end(), std::forward_iterator_tag());
    }

    template<typename InputIt>
    Vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : Base(std::distance(first, last), alloc)
    {
        range_initialize(first, last, std::input_iterator_tag());
    }

private:
    void default_initialize(size_type count)
    {
        data.finish = cyy::uninitialize_default_n_a(data.start, n, allocator_type());
    }

    void fill_initialize(size_type count, const value_type& value)
    {
        data.finish = cyy::uninitialize_fill_n_a(data.start, n, value, allocator_type());
    }

    void copy_initialize(const Vector& other)
    {
        data.finish = cyy::uninitialized_copy_a(other.begin(), other.end(), data.start, allocator_type())
    }

    template<typename InputIt>
    void range_initialize(InputIt first, IputIt last, std::input_iterator_tag)
    {
        for (; first != last;)
        {
            emplace_back(*first);
        }
    }

    template<typename ForwardIt>
    void range_initialize(ForwardIt first, ForwardIt last, std::forward_iterator_tag)
    {
        data.finish = cyy::uninitialized_copy_a(first, last, data.first, allocator_type());
    }

}

} // namespace cyy
#endif // !VECTOR_H
