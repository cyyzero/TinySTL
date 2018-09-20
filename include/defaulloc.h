#ifndef DEFAULLOC_H
#define DEFAULLOC_H

#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace cyyzero
{
    template <typename T>
    class allocator
    {
    public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        // constructors
        allocator() noexcept { }

        allocator(const allocator&) noexcept { }

        template<typename P>
        allocator(const allocator<P>&) noexcept { }

        // destructor
        ~allocator()  noexcept { }

        template<typename T1>
        struct rebind
        {
            using other = allocator<T1>;
        };

        pointer allocate(size_type n, void *p = 0)
        {
            if (n > this->max_size())
            {
                throw std::bad_alloc();
            }
            return static_cast<pointer>(::operator new(n * sizeof(T)));
        }

        void deallocate(pointer p, size_type)
        {
            ::operator delete(p);
        }

        pointer address(reference x) const noexcept
        {
            return std::addressof(x);
        }

        const_pointer address(const_reference x) const noexcept
        {
            return std::addressof(x);
        }

        size_type max_size() const
        {
            return std::size_t(-1) / sizeof(T);
        }

        template<typename U, typename... Args>
        void construct(U* p, Args&&... args) const
        {
            ::new((void*)p) U(std::forward<Args>(args)...);
        }

        template<typename U>
        void destroy(U* p)
        {
            p->~U();
        }
    };

    template<typename T>
    inline bool operator==(const allocator<T>&, const allocator<T>&) noexcept
    {
        return true;
    }

    template<typename T1, typename T2>
    inline bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept
    {
        return true;
    }

    template<typename T>
    inline bool operator!=(const allocator<T>&, const allocator<T>&) noexcept
    {
        return false;
    }

    template<typename T1, typename T2>
    inline bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept
    {
        return false;
    }

    template<>
    class allocator<void>
    {
    public:
        using pointer = void*;
    };
}

#endif
