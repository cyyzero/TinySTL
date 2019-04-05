#ifndef __INTEGER_SEQUENCE
#define __INTEGER_SEQUENCE

#include <type_traits>

namespace cyy
{

template<typename T, T... Ints>
class Integer_sequence
{
    static_assert(std::is_integral<T>::value,
                  "T must be integer type");
    static constexpr std::size_t size() noexcept
    {
        return sizeof...(Ints);
    }
};

template<typename T, T... Ints>
struct Make_integer_sequence_impl;

template<typename T, T Sp, T Ep, T... Ints>
struct Make_integer_sequence_impl<T, Sp, Ep, Ints...>
    : public Make_integer_sequence_impl<T, Sp, Ep-1, Ep-1, Ints...>
{
};

template<typename T, T Sp, T... Ints>
struct Make_integer_sequence_impl<T, Sp, Sp , Ints...>
{
    using type = Integer_sequence<T, Ints...>;
};

template<std::size_t... Ints>
using Index_sequence = Integer_sequence<std::size_t, Ints...>;

template<typename T, T N>
using Make_integer_sequence = typename Make_integer_sequence_impl<T, 0, N>::type;

template<std::size_t N>
using Make_index_sequence = Make_integer_sequence<std::size_t, N>;

template<class... T>
using Index_sequence_for = Make_index_sequence<sizeof...(T)>;

} // namespace cyy
#endif // __INTEGER_SEQUENCE
