#include <vector>
#include <memory>
#include "tuple.h"

int main()
{
    cyyzero::Tuple<int, std::string, double> t1;
    // std::cout << "Value-initialized: "; print(t1);
    cyyzero::Tuple<int, std::string, double> t2(42, "Test", -3.14);
    // std::cout << "Initialized with values: "; print(t2);
    cyyzero::Tuple<char, std::string, int> t3(t2);
    // std::cout << "Implicitly converted: "; print(t3);
    cyyzero::Tuple<int, double> t4(cyyzero::Tuple<int, double>(42, 3.14));
    // std::cout << "Constructed from a pair"; print(t4);
 
    // given Allocator my_alloc with a single-argument constructor my_alloc(int)
    // use my_alloc(1) to allocate 10 ints in a vector
    // std::vector<int, std::alloc> v(10, 1, std::alloc(1));
    // use my_alloc(2) to allocate 10 ints in a vector in a tuple
    // cyyzero::Tuple<int, std::vector<int, my_alloc>, double> t5(std::allocator_arg,
    //                                                       my_alloc(2), 42, v,  -3.14);
}