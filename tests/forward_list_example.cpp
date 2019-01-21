
#include <string>
#include <iostream>
 
#include "forward_list.h"

template<typename T>
std::ostream& operator<<(std::ostream& s, const cyy::Forward_list<T>& v) {
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
    // c++11 initializer list syntax:
    cyy::Forward_list<std::string> words1 {"the", "frogurt", "is", "also", "cursed"};
    std::cout << "words1: " << words1 << '\n';

    // words2 == words1
    cyy::Forward_list<std::string> words2(words1.begin(), words1.end());
    std::cout << "words2: " << words2 << '\n';

    // words3 == words1
    cyy::Forward_list<std::string> words3(words1);
    std::cout << "words3: " << words3 << '\n';

    // words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
    cyy::Forward_list<std::string> words4(5, "Mo");
    std::cout << "words4: " << words4 << '\n';

    cyy::Forward_list<std::string> words5(std::move(words1));
    std::cout << "words5: " << words5 << '\n';
    std::cout << "words1: " << words1 << '\n';
}