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

struct C
{
    friend std::ostream& operator<<(std::ostream& s, const C&)
    {
        s << "C";
        return s;
    }
    C()
    {
        std::cout << "C ctor\n";
    }

    C(const C&)
    {
        std::cout << "C copy ctor\n";
    }

    ~C()
    {
        std::cout << "C dtor\n";
    }
};

int main()
{
    std::cout << "Test for List::iterator\n";
    {
        cyy::detail::List_iterator<int> it1;
        cyy::detail::List_const_iterator<int> it2;

        std::cout << (it1 == it2) << (it1 != it2)
                  << (it1 == it1) << (it1 != it1)
                  << (it2 == it2) << (it2 != it2)
                  << (it2 == it1) << (it2 != it1) << "\n";
    }

    std::cout << "\nTest for constructors and destructor\n";
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

        List<C> Cs(6, C());
        std::cout << Cs << '\n';
    }

    std::cout << "\nTest for operator=()\n";
    {
        auto display_sizes = [] (
            const List<int>& nums1,
            const List<int>& nums2,
            const List<int>& nums3)
        {
            std::cout << "nums1: " << nums1.size() 
                    << " nums2: " << nums2.size()
                    << " nums3: " << nums3.size() << '\n';
        };

        List<int> nums1 {3, 1, 4, 6, 5, 9};
        List<int> nums2; 
        List<int> nums3;
    
        assert(nums1.size() == 6);
        assert(nums2.size() == 0);
        assert(nums3.size() == 0);
        std::cout << "Initially:\n";
        display_sizes(nums1, nums2, nums3);
    
        // copy assignment copies data from nums1 to nums2
        nums2 = nums1;
        assert(nums1.size() == 6);
        assert(nums2.size() == 6);
        assert(nums3.size() == 0);
        std::cout << "After assigment:\n"; 
        display_sizes(nums1, nums2, nums3);
    
        // move assignment moves data from nums1 to nums3,
        // modifying both nums1 and nums3
        nums3 = std::move(nums1);
        assert(nums1.size() == 0);
        assert(nums2.size() == 6);
        assert(nums3.size() == 6);
        std::cout << "After move assigment:\n"; 
        display_sizes(nums1, nums2, nums3);
    }

    std::cout << "\nTest for assign()\n";
    {
        List<char> characters(2);
        characters.assign(5, 'a');
        // assert(characters == List<char>(5, 'a'));
        std::cout << characters << "\n";
        characters.assign({'f', 'k'});
        // assert(characters == List<char>{'f', 'k'});
        std::cout << characters << '\n';
    }

    std::cout << "\nTest for front() and back()\n";
    {
        List<char> letters {'o', 'm', 'g', 'w', 't', 'f'};
        assert(letters.front() == 'o');
        assert(letters.back() == 'f');
        std::cout << "The first character is: " << letters.front() << '\n';
        std::cout << "The last character is: " << letters.back() << '\n';
    }

    std::cout << "\nTest for empty()\n";
    {
        List<int> numbers;
        assert(numbers.empty());
        std::cout << "Initially, numbers.empty(): " << numbers.empty() << '\n';
    
        numbers.push_back(42);
        numbers.push_back(13317);
        assert(!numbers.empty());
        std::cout << "After adding elements, numbers.empty(): " << numbers.empty() << '\n';
    }

    std::cout << "\nTest for size()\n";
    {
        List<int> nums {1, 3, 5, 7};
        assert(nums.size() == 4);
        std::cout << "nums contains " << nums.size() << " elements.\n";
    }

    std::cout << "\nTest for max_size()\n";
    {
        List<char> s;
        std::cout << "Maximum size of a 'list' is " << s.max_size() << "\n";
    }

    std::cout << "\nTest for clear()\n";
    {
        List<int> l(10);
        assert(l.size() == 10);
        l.clear();
        assert(l.size() == 0);
        std::cout << "After clear(), size of l is " << l.size() << std::endl;
    }

    std::cout << "\nTest for erase()\n";
    {
        List<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::cout << c << '\n';
        // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        c.erase(c.begin());
        // assert(c == List<int>{1, 2, 3, 4, 5, 6, 7, 8, 9});
        std::cout << c << '\n';
        // [1, 2, 3, 4, 5, 6, 7, 8, 9]

    
        List<int>::iterator range_begin = c.begin();
        List<int>::iterator range_end = c.begin();
        std::advance(range_begin,2);
        std::advance(range_end,5);
    
        c.erase(range_begin, range_end);

        // assert(c == List<int>{1, 2, 6, 7, 8, 9});
        std::cout << c << '\n';
        // [1, 2, 6, 7, 8, 9]
    
        // Erase all even numbers (C++11 and later)
        for (auto it = c.begin(); it != c.end(); ) {
            if (*it % 2 == 0) {
                it = c.erase(it);
            } else {
                ++it;
            }
        }

        // assert(c == List<int>{1, 7, 9});
        std::cout << c << '\n';
        // [1, 7, 9]
    }

    std::cout << "\nTest for pop_back()\n";
    {
        List<int> numbers;
        std::cout << numbers << '\n';
    
        numbers.push_back(5);
        numbers.push_back(3);
        numbers.push_back(4);
        // assert(numbers == List<int>{5, 3, 4});
    
        std::cout << numbers << '\n'; 
    
        numbers.pop_back();
    
        // assert(numbers == List<int>{5, 3});
        std::cout << numbers << '\n'; 
    }

    std::cout << "\nTest for push_front(), emplace_front() and pop_front()\n";
    {
        List<std::string> l{"fxxk"};
        l.push_front("the");
        l.emplace_front("what");
        std::cout << l << '\n';
        l.pop_front();
        std::cout << l << '\n';
    }

    std::cout << "\nTest for swap()\n";
    {
        List<int> l1{1,2,3};
        List<int> l2{4,5};
        std::cout << l1 << '\n' << l2 << '\n';
        l1.swap(l2);
        // assert(l1 == List<int>{4, 5});
        // assert(l1.size() == 2);
        // assert(l2 == List<int>{1,2,3});
        // assert(l2.size() == 3);
        std::cout << l1 << '\n' << l2 << '\n';
    }

    std::cout << "\nTest for resize()\n";
    {
        List<int> c = {1, 2, 3};
        std::cout << "The list holds: " << c << '\n';
        c.resize(5);
        // assert(c == List<int>{1, 2, 3, 0, 0});
        std::cout << "After resize up to 5: " << c << '\n';
        c.resize(2);
        // assert(c == List<int>{1, 2});
        std::cout << "After resize down to 2: " << c << '\n';
    }

    std::cout << "\nTest for merge()\n";
    {
        // List<int> list1 = { 5,9,0,1,3 };
        // List<int> list2 = { 8,7,2,6,4 };
    
        // list1.sort();
        // list2.sort();

        List<int> list1 = { 0,1,3,5,9 };
        List<int> list2 = { 2,4,6,7,8 };
        std::cout << "list1:  " << list1 << "\n";
        std::cout << "list2:  " << list2 << "\n";
        list1.merge(list2);
        std::cout << "merged: " << list1 << "\n";

        // assert(list1 == List<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        // assert(list2 == List<int>{});
    }

    std::cout << "\nTest for splice()\n";
    {
        List<int> list1 = { 1, 2, 3, 4, 5 };
        List<int> list2 = { 10, 20, 30, 40, 50 };
    
        auto it = list1.begin();
        std::advance(it, 2);

        list1.splice(it, list2);
        assert(list1.size() == 10);
        assert(list2.size() == 0);
        // assert(list1 == List<int>{1, 2, 10, 20, 30, 40, 50, 3, 4, 5});
        // assert(list2 == List<int>{});
        std::cout << "list1: " << list1 << "\n";
        std::cout << "list2: " << list2 << "\n";
    
        list2.splice(list2.begin(), list1, it, list1.end());
    
        assert(list1.size() == 7);
        assert(list2.size() == 3);
        // assert(list1 == List<int>{1, 2, 10, 20, 30, 40, 50});
        // assert(list2 == List<int>{3, 4, 5});
        std::cout << "list1: " << list1 << "\n";
        std::cout << "list2: " << list2 << "\n";
    }

}