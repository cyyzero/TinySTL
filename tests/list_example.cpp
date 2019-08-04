#include "list.h"

#include <string>
#include <iostream>
#include <cassert>

using namespace cyy;
using namespace std;

template<typename T>
std::ostream& operator<<(std::ostream& s, const List<T>& v) {
    s.put('[');
    char comma[3] = {'\0', ' ', '\0'};
    for (const auto& e : v) {
        s << comma << e;
        comma[0] = ',';
    }
    return s << ']';
}

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
    
    std::cout << "\nTest for constructors\n";
    {
        // c++11 initializer list syntax:
        List<std::string> words1 {"the", "frogurt", "is", "also", "cursed"};
        std::cout << "words1: " << words1 << '\n';
    
        // words2 == words1
        List<std::string> words2(words1.begin(), words1.end());
        // assert(words1 == words2);
        std::cout << "words2: " << words2 << '\n';
    
        // words3 == words1
        List<std::string> words3(words1);
        // assert(words1 == words3);
        std::cout << "words3: " << words3 << '\n';
    
        // words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
        List<std::string> words4(5, "Mo");
        // assert(words4 == List<std::string>{"Mo", "Mo", "Mo", "Mo", "Mo"});
        std::cout << "words4: " << words4 << '\n';
    }
}