#ifndef DEFAULT_ALLOC_TEMPLATE_H
#define DEFAULT_ALLOC_TEMPLATE_H

#include "__malloc_alloc_template.h"

enum {__ALIGN = 8};                          // upper size
enum {__MAX_BYTE = 128};                     // max size of block
enum {__NFREELISTS = __MAX_BYTE/__ALIGN};    // size of free-lists

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

    // 16 free-lists
    static obj * volatile free_list[__NFREELISTS];

    // decide which free-list to ues according to size of the block
    static std::size_t FREELIST_INDEX(std::size_t bytes)
    {
        return ((bytes + __ALIGN-1)/__ALIGN - 1);
    }

    // return an object in size of n
    static void *refill(std::size_t n)
    {
        int nobjs = 10;
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
        *my_free_list = nexe_obj = (obj *)(chunk + n);
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


    static char *chunk_alloc(std::size_t size, int &nobjs)
    {
        
    }

    // Chunk allocation state
    static char *start_free;                              // Start of memory pool, only changed in chunk_alloc()
    static char *end_free;                                // End of memory pool, only changeed in chunk_alloc()
    static std::size_t heap_size;

public:
    static void *allocate(std::size_t n)
    {
        obj * volatile * my_free_list;
        obj * result;

        // if larger than 128, then call the malloc_alloc::allocate
        if (n > (std::size_t) __MAX_BYTES)
        {
            return malloc_alloc::allocate(n);
        }

        // Look for the adequate free_list
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr)
        {
            // Refill free_list if fail to look for adequate one
            void *r = refill(ROUND_UP(n));
            return r;
        }

        // Adjust free_list
        *my_free_list = result->free_list_link;
        return result;
    }

    static void deallocate(void *p, std::size_t n)
    {
        obj *q = (obj *)p;
        obj * volatile * my_free_list;

        // Call malloc_alloc::deallocate if size > 128
        if (n > (std::size_t) __MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        // Look for free_list
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;

    }

    static void *reallocate(void *p, std::size_t old_sz, std::size_t new_sz)
    {
        
    }
}

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = nullptr;

template<bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = nullptr;

template<bool threads, int inst>
std::size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

#endif