#include <memory>
#include <string>
#include <iostream>
#include "Allocator/defaulloc.h"
int main()
{
    cyyzero::allocator<std::string> a2;
    std::string* s = a2.allocate(2); // 2 个 string 的空间
 
    a2.construct(s, "foo");
    a2.construct(s + 1, "bar");
 
    std::cout << s[0] << ' ' << s[1] << '\n';
 
    a2.destroy(s);
    a2.destroy(s + 1);
    a2.deallocate(s, 2);
}
