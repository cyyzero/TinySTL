#include "tuple.h"
#include "array.h"
#include "integer_sequence.h"

#include <iostream>
#include <utility>
 
// Convert array into a tuple
template<typename Array, std::size_t... I>
auto a2t_impl(const Array& a, cyy::Index_sequence<I...>)
{
    return cyy::make_tuple(a[I]...);
}
 
template<typename T, std::size_t N, typename Indices = cyy::Make_index_sequence<N>>
auto a2t(const cyy::Array<T, N>& a)
{
    return a2t_impl(a, Indices{});
}
 
// pretty-print a tuple
 
template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch,Tr>& os,
                      const Tuple& t,
                      cyy::Index_sequence<Is...>)
{
    ((os << (Is == 0? "" : ", ") << cyy::get<Is>(t)), ...);
}
 
template<class Ch, class Tr, class... Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                 const cyy::Tuple<Args...>& t)
{
    os << "(";
    print_tuple_impl(os, t, cyy::Index_sequence_for<Args...>{});
    return os << ")";
}
 
int main()
{
    cyy::Array<int, 4> array = {1,2,3,4};
 
    // convert an array into a tuple
    auto tuple = a2t(array);
    static_assert(std::is_same<decltype(tuple),
                               cyy::Tuple<int, int, int, int>>::value, "");
 
    // print it to cout
    std::cout << tuple << '\n';
}