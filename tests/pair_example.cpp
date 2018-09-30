#include <utility>
#include <string>
#include <complex>
#include <functional>
#include <iostream>
#include "pair.h"

int main()
{
    cyy::pair<int, float> p1;
    std::cout << "Value-initialized: "
              << p1.first << ", " << p1.second << '\n';
 
    cyy::pair<int, double> p2(42, 0.123);
    std::cout << "Initialized with two values: "
              << p2.first << ", " << p2.second << '\n';
 
    cyy::pair<char, int> p4(p2);
    std::cout << "Implicitly converted: "
              << p4.first << ", " << p4.second << '\n';
 
    // cyy::pair<std::complex<double>, std::string> p6(
    //                 std::piecewise_construct, 
    //                 std::forward_as_tuple(0.123, 7.7),
    //                 std::forward_as_tuple(10, 'a'));
    // std::cout << "Piecewise constructed: "
    //           << p6.first << ", " << p6.second << '\n';
    {
        int n = 1;
        int a[5] = {1, 2, 3, 4, 5};
    
        // 从二个 int 建造 pair
        auto p1 = cyy::make_pair(n, a[1]);
        std::cout << "The value of p1 is "
                << "(" << p1.first << ", " << p1.second << ")\n";
    
        // 从到 int 的引用和数组（退化成指针）建造 pair
        auto p2 = cyy::make_pair(std::ref(n), a);
        n = 7;
        std::cout << "The value of p2 is "
                << "(" << p2.first << ", " << *(p2.second + 2) << ")\n";
    }

}