#include <memory>
#include <string>
#include <iostream>
#include "defaulloc.h"
int main()
{
    cyy::allocator<int> a2;
    int* s = a2.allocate(2); // 2 个 string 的空间
 
    a2.construct(s, 1);
    a2.construct(s + 1, 2);
 
    std::cout << s[0] << ' ' << s[1] << '\n';
 
    a2.destroy(s);
    a2.destroy(s + 1);
    a2.deallocate(s, 2);
}
