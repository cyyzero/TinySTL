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

template<typename T, T Sp, typename Seq, T Ep>
struct Make_integer_sequence_impl;

template<typename T, T Sp, T... Ints , T Ep>
struct Make_integer_sequence_impl<T, Sp, Integer_sequence<T, Ints...> , Ep>
    : public Make_integer_sequence_impl<T, Sp+1, Integer_sequence<T, Ints..., Sp> , Ep>
{
};

template<typename T, T Sp, T... Ints>
struct Make_integer_sequence_impl<T, Sp, Integer_sequence<T, Ints...>, Sp>
{
    using type = Integer_sequence<T, Ints...>;
};

template<std::size_t... Ints>
using Index_sequence = Integer_sequence<std::size_t, Ints...>;

template<typename T, T N>
using Make_integer_sequence = typename Make_integer_sequence_impl<T, 0, Integer_sequence<T>, N>::type;

template<std::size_t N>
using Make_index_sequence = Make_integer_sequence<std::size_t, N>;

template<class... T>
using Index_sequence_for = Make_index_sequence<sizeof...(T)>;

} // namespace cyy
#endif // __INTEGER_SEQUENCE
