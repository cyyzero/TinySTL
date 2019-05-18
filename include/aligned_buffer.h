#ifndef ALIGNED_BUFFER_H
#define ALIGNED_BUFFER_H

#include <type_traits>

namespace cyy
{
// aligned buffer used in Fwd_list_node
template<typename T>
struct aligned_buffer
    : public std::aligned_storage<sizeof(T), alignof(T)>
{
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;

    aligned_buffer() = default;

    void* address() noexcept
    {
        return static_cast<void*>(&storage);
    }

    const void* address() const noexcept
    {
        return static_cast<const void*>(&storage);
    }

    T* pointer() noexcept
    {
        return static_cast<T*>(address());
    }

    const T* pointer() const noexcept
    {
        return static_cast<const T*>(address());
    }
}; // class aligned_buffer
} // namespace cyy
#endif // !ALIGNED_BUFFER_H
