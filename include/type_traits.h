#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <type_traits>

namespace cyy
{

// add reference when type is reference_wrapper
template<typename T>
struct strip_reference_wrapper
{
    using type = T;
};

template<typename T>
struct strip_reference_wrapper<std::reference_wrapper<T>>
{
    using type = T&;
};

template<typename T>
struct decay_and_strip
{
    using type = typename strip_reference_wrapper<typename std::decay<T>::type>::type;
};

} // namespace cyy

#endif // TYPE_TRAITS
