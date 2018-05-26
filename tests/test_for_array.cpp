#include "../Container/array.h"

#include <string>
#include <iterator>
#include <iostream>
#include <algorithm>
 
int main()
{
    // 用聚合初始化构造
    cyyzero::array<int, 3> a1{ {1, 2, 3} }; // C++11 中要求双花括号（ C++14 中不要求）
    cyyzero::array<int, 3> a2 = {1, 2, 3};  // 决不要求在 = 后
    cyyzero::array<std::string, 2> a3 = { std::string("a"), "b" };
    
    std::cout << "a1[1]"<< a1[1] << std::endl;
    // 支持容器操作
    std::sort(a1.begin(), a1.end());
    std::reverse_copy(a2.begin(), a2.end(), 
                      std::ostream_iterator<int>(std::cout, " "));
 
    std::cout << '\n';
 
    // 支持带范围 for 循环
    for(const auto& s: a3)
        std::cout << s << ' ';
    
    std::cout << "\n" << a1.size() << std::endl;
    for (const auto& i : a1)
        std::cout << i << " ";
    std::cout << std::endl;
    for (auto it = a1.begin(); it != a1.end(); ++it)
        std::cout << *it << std::endl;
    a1.fill(211);
    for (const auto& i: a1)
        std::cout << i << ' ';
    std::cout << std::endl;
    a1.front() = 3;
    a1.back() = 4;
    for (const auto i: a1)
        std::cout << i << " ";
}