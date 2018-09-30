#ifndef SGI_ALLOC_H
#define SGI_ALLOC_H

#include <new>
#include <cstdlib>

#define __THROW_BAD_ALLOC throw std::bad_alloc

namespace cyy
{
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

namespace {
    constexpr static std::size_t __ALIGN = 8;                          // upper size
    constexpr static std::size_t __MAX_BYTES = 128;                     // max size of block
    constexpr static std::size_t __NFREELISTS = __MAX_BYTE/__ALIGN;    // size of free-lists
}

tmeplate<bool threads, int inst>
class __default_alloc_template
{
private:
    static std::size_t ROUND_UP(std::size_t bytes)
    {
        return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
    }

    union obj
    {
        union obj * free_list_link;
        char client_data[1];                   /* The client sees this. */
    }

    // decide which free-list to ues according to size of the block
    static std::size_t FREELIST_INDEX(std::size_t bytes)
    {
        return ((bytes + __ALIGN - 1)/__ALIGN - 1);
    }

    // Returns an object of size n, and optionally adds to size n free list.
    // We assume that n is properly aligned.
    static void *refill(std::size_t n)
    {
        int nobjs = 20;
        char * chunk = chunk_alloc(n, nobjs);
        obj * volatile * my_free_list;
        obj * result;
        obj * current_obj, * next_obj;
        int i;

        if (nobj == 1)
        {
            return chunk;
        }
        my_free_list = free_list + FREELIST_INDEX(n);

        result = (obj *)chunk;
        *my_free_list = next_obj = (obj *)(chunk + n);
        for (i = 1; ; ++i)
        {
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if (nobjs - 1 == i)
            {
                current_obj->free_list_link = nullptr;
                break;
            }
            else
            {
                current_obj->free_list_link = next_obj;
            }
        }
        return result;
    }

    // Allocates a chunk for nobjs of size size.  nobjs may be reduced
    // if it is inconvenient to allocate the requested number.
    static char *chunk_alloc(std::size_t size, int &nobjs)
    {
        char * result;
        std::size_t total_bytes = size * nobjs;
        std::size_t bytes_left  = end_free - start_free;

        if (bytes_left >= total_bytes)
        {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else
        {
            std::size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            // Try to make use of the left-over piece.
            if (bytes_left > 0)
            {
                obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            start_free = (char *)malloc(bytes_to_get);
            
            if (start_free == nullptr)
            {
                std::size_t i;
                obj * volatile * my_free_list , *p;
                // Try to make do with what we have.  That can't
                // hurt.  We do not try smaller requests, since that tends
                // to result in disaster on multi-process machines.
                for (i = size; i < __MAX_BYTES; i += __ALIGN)
                {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != nullptr)
                    {
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                        // Any leftover piece will eventually make it to the
                        // right free list.
                    }
                }
                end_free = nullptr;
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
                // This should either throw an
                // exception or remedy the situation.  Thus we assume it
                // succeeded.
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }


    // 16 free-lists
    static obj * volatile free_list[__NFREELISTS];
    // Chunk allocation state
    static char *start_free;                              // Start of memory pool, only changed in chunk_alloc()
    static char *end_free;                                // End of memory pool, only changeed in chunk_alloc()
    static std::size_t heap_size;

public:

    // n must be > 0
    static void *allocate(std::size_t n)
    {
        void * result = nullptr;
        // if larger than 128, then call the malloc_alloc::allocate
        if (n > __MAX_BYTES)
        {
            result = malloc_alloc::allocate(n);
        }
        else
        {
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(n);
            result = *my_free_list;
            if (result == nullptr)
            {
                // Refill free_list if fail to look for adequate one
                result = refill(ROUND_UP(n));
            }
            else
            {
                *my_free_list = (*my_free_list)->free_list_link;
            }
        }
        return result;
    }

    /* p may not be 0 */
    static void deallocate(void * p, std::size_t n)
    {
        // Call malloc_alloc::deallocate if size > 128
        if (n > __MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
        }
        else
        {
            // Look for free_list
            obj *q = (obj *)p;
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    }

    static void *reallocate(void *p, std::size_t old_sz, std::size_t new_sz)
    {
        if (old_sz > __MAX_BYTES && new_sz > __MAX_BYTES)
        {
            return realloc(p, new_sz);
        }
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
        {
            return p;
        }
        
        void * result;
        std::size_t copy_sz;
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }
}

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
std::size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



template <class Tp, class Alloc>
class simple_alloc
{
public:
    static Tp* allocate(size_t n)
    {
        return n == 0 ? (Tp*) Alloc::allocate(n * sizeof(Tp));
    }

    static Tp* allocate(void)
    {
        return (*Tp) Alloc::allocate(sizeof(Tp));
    }

    static void deallocate(Tp * p, std::size_t n)
    {
        if (n == 0)
            Alloc::deallocate(p, n * sizeof(Tp));
    }

    static void deallocate(Tp* p)
    {
        Alloc::deallocate(p, sizeof(Tp));
    }
}
}

#endif /*SGI_ALLOC_H */