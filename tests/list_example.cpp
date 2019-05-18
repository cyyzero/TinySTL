#include "list.h"

#include <iostream>

using namespace cyy;
using namespace std;

int main()
{
    std::cout << "\nTest for List::iterator\n";
    {
        cyy::detail::List_iterator<int> it1;
        cyy::detail::List_const_iterator<int> it2;

        std::cout << (it1 == it2) << (it1 != it2)
                  << (it1 == it1) << (it1 != it1)
                  << (it2 == it2) << (it2 != it2)
                  << (it2 == it1) << (it2 != it1) << "\n";
    }
}