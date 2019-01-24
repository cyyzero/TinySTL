
#include <string>
#include <iostream>
 
#include "vector.h"
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

void display_sizes(const cyy::Forward_list<int> &nums1,
                   const cyy::Forward_list<int> &nums2,
                   const cyy::Forward_list<int> &nums3)
{
    std::cout << "nums1: " << std::distance(nums1.begin(), nums1.end()) 
              << " nums2: " << std::distance(nums2.begin(), nums2.end())
              << " nums3: " << std::distance(nums3.begin(), nums3.end()) << '\n';
}

using namespace cyy;

int main() 
{
    std::cout << "Test for ctors and dtors:\n";
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

    std::cout << "\nTest for operator=:\n";
    {
        cyy::Forward_list<int> nums1 {3, 1, 4, 6, 5, 9};
        cyy::Forward_list<int> nums2 {1, 2, 3}; 
        cyy::Forward_list<int> nums3 {1, 2, 3, 4, 5, 6, 7, 8, 9};

        std::cout << "Initially:\n";
        display_sizes(nums1, nums2, nums3);
        // output 6, 3, 9

        // copy assignment copies data from nums1 to nums2
        nums1 = nums2;

        std::cout << "After assigment1:\n"; 
        display_sizes(nums1, nums2, nums3);
        // output 3, 3, 9

        nums1 = nums3;
        std::cout << "After assigment2:\n";
        display_sizes(nums1, nums2, nums3);
        // output 9, 3, 9

        // move assignment moves data from nums1 to nums3,
        // modifying both nums1 and nums3
        nums2 = std::move(nums1);

        std::cout << "After move assigment:\n"; 
        display_sizes(nums1, nums2, nums3);
        // output 0, 9, 9
    }

    std::cout << "\nTest for assign:\n";
    {
        cyy::Forward_list<char> characters;

        characters.assign(5, 'a');

        for (char c : characters) {
            std::cout << c << '\n';
        } 
    }

    std::cout << "\nTest for front:\n";
    {
        cyy::Forward_list<char> letters {'o', 'm', 'g', 'w', 't', 'f'};
    
        // if (!letters.empty()) {
            std::cout << "The first character is: " << letters.front() << '\n';
        // }  
    }

    std::cout << "\nTest for begin/end:\n";
    {
        cyy::Forward_list<int> ints {1, 2, 4, 8, 16};
        cyy::Forward_list<std::string> fruits {"orange", "apple", "raspberry"};
        cyy::Forward_list<char> empty;
    
        // Sums all integers in the forward_list ints (if any), printing only the result.
        int sum = 0;
        for (auto it = ints.cbegin(); it != ints.cend(); it++)
            sum += *it;
        std::cout << "Sum of ints: " << sum << "\n";
    
        // Prints the first fruit in the forward_list fruits, without checking if there is one.
        std::cout << "First fruit: " << *fruits.begin() << "\n";
    
        if (empty.begin() == empty.end())
            std::cout << "forward_list 'empty' is indeed empty.\n";
    }

    std::cout << "\nTest for empty:\n";
    {
        cyy::Forward_list<int> numbers;
        std::cout << "Initially, numbers.empty(): " << numbers.empty() << '\n';
    
        numbers.push_front(42);
        numbers.push_front(13317); 
        std::cout << "After adding elements, numbers.empty(): " << numbers.empty() << '\n';
    }

    std::cout << "\nTest for max_size:\n";
    {
        cyy::Forward_list<char> s;
        std::cout << "Maximum size of a 'forward_list' is " << s.max_size() << "\n";
    }

    std::cout << "\nTest for insert_after:\n";
    {
        cyy::Forward_list<std::string> words {"the", "frogurt", "is", "also", "cursed"};
        std::cout << "words: " << words << '\n';                                    
    
        // insert_after (2)                                                         
        auto beginIt = words.begin();                                               
        words.insert_after(beginIt, "strawberry");                                  
        std::cout << "words: " << words << '\n';                                    
    
        // insert_after (3)                                                         
        auto anotherIt = beginIt;                                                   
        ++anotherIt;                                                                
        anotherIt = words.insert_after(anotherIt, 2, "strawberry");                 
        std::cout << "words: " << words << '\n';                                    
    
        // insert_after (4)
        Vector<std::string> V = { "apple", "banana", "cherry"};                
        anotherIt = words.insert_after(anotherIt, V.begin(), V.end());              
        std::cout << "words: " << words << '\n';                                    
    
        // insert_after (5)                                                         
        words.insert_after(anotherIt, {"jackfruit", "kiwifruit", "lime", "mango"});
        std::cout << "words: " << words << '\n';                     
    }
}