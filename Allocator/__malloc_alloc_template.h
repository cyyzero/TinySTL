#ifndef MALLOC_ALLOC_TEMPLATE_H
#define MALLOC_ALLOC_TEMPLATE_H

#if 0
# include <new>
# define __THROW_BAD_ALLOC throw std::bad_alloc
#elif !define(__THROW_BAD_ALLOC)
# include <iostream>
# define __THROW_BAD_ALLOC std::cerr << "Out of memory" << std::endl; eixt(1);
# endif

template <int inst>
class __malloc_alloc_template
{
private:
    // oom : out of memory
    static void *oom_malloc(std::size_t);
    static void *oom_realloc(void *, std::size_t);
    static void (* __malloc_alloc_oom_handler)();

public:

    static void *allocate(std::size_t n)
    {
        void *result = malloc(n);
        if (result == nullptr)
            result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, std::size_t /* n */)
    {
        free(p);
    }

    static void * reallocate(void *p, std::size_t /*old_sz*/, std::size_t new_sz)
    {
        void *result = realloc(p, new_sz);
        if (result == nullptr)
            result = oom_realloc(p, new_sz);
        return result;
    }

    static void (* set_malloc_handler(void (*f)()))();
    {
        void (* old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return (old);
    }
}

template<int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = nullptr;

template<int inst>
void * __malloc_alloc_template<inst>::oom_malloc(std::size_t n)
{
    void (* my_malloc_handler)();
    void *result;
    for (;;)                                                 // Keep trying to releasing and mallocing...
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr)
        {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();                             // call the handler trying to release the memory
        result = malloc(n);                                 // retry malloc memory
        if (result)
            return result;
    }
}

template<int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, std::size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    for (;;)
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr)
        {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result)
            return result;
    }
}

using malloc_alloc = __malloc_alloc_template<0>;

#endif