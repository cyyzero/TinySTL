#include <string>
#include <memory>
#include <vector>
#include <iomanip>
#include <iostream>
#include "vector.h"
 
template<typename T, typename Alloc>
std::ostream& operator<<(std::ostream& s, const cyy::Vector<T, Alloc>& v) {
    s.put('[');
    char comma[3] = {'\0', ' ', '\0'};
    for (const auto& e : v) {
        s << comma << e;
        comma[0] = ',';
    }
    return s << ']';
}

using namespace cyy;
void display_sizes(const Vector<int>& nums1,
                const Vector<int>& nums2,
                const Vector<int>& nums3)
{
    std::cout << "nums1: " << nums1.size() 
              << "nums2: " << nums2.size()
              << "nums3: " << nums3.size() << '\n';
}

// minimal C++11 allocator with debug output
template <class Tp>
struct NAlloc {
    typedef Tp value_type;
    NAlloc() = default;
    template <class T> NAlloc(const NAlloc<T>&) {}
    Tp* allocate(std::size_t n) {
        n *= sizeof(Tp);
        std::cout << "allocating " << n << " bytes\n";
        return static_cast<Tp*>(::operator new(n));
    }
    void deallocate(Tp* p, std::size_t n) {
        std::cout << "deallocating " << n*sizeof*p << " bytes\n";
        ::operator delete(p);
    }
};

template <class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&) { return true; }
template <class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&) { return false; }

int main() 
{
    {

    // c++11 初始化器列表语法：
    Vector<std::string> words1 {"the", "frogurt", "is", "also", "cursed"};
    std::cout << "words1: " << words1 << '\n';

    // words2 == words1
    cyy::Vector<std::string> words2(words1.begin(), words1.end());
    std::cout << "words2: " << words2 << '\n';

    // words3 == words1
    cyy::Vector<std::string> words3(words1);
    std::cout << "words3: " << words3 << '\n';

    // words4 为 {"Mo", "Mo", "Mo", "Mo", "Mo"}
    cyy::Vector<std::string> words4(5, "Mo");
    std::cout << "words4: " << words4 << '\n';
    }

    {
        Vector<int> nums1 {3, 1, 4, 6, 5, 9};
        Vector<int> nums2; 
        Vector<int> nums3;
    
        std::cout << "Initially:\n";
        display_sizes(nums1, nums2, nums3);
    
        // copy assignment copies data from nums1 to nums2
        nums2 = nums1;
    
        std::cout << "After assigment:\n"; 
        display_sizes(nums1, nums2, nums3);
    
        // move assignment moves data from nums1 to nums3,
        // modifying both nums1 and nums3
        nums3 = std::move(nums1);
    
        std::cout << "After move assigment:\n"; 
        display_sizes(nums1, nums2, nums3);
    }

    std::cout << "\nTest for reserve:\n";
    {
        int sz = 100;
        std::cout << "using reserve: \n";
        {
            cyy::Vector<int, NAlloc<int>> v1;
            v1.reserve(sz);
            // std::cout << v1.size() << " " << v1.capacity() << "\n";
            // for(int n = 0; n < sz; ++n)
            //     v1.push_back(n);
        }
        std::cout << "not using reserve: \n";
        {
            cyy::Vector<int, NAlloc<int>> v1;
            // std::cout << v1.size() << " " << v1.capacity() << "\n";
            // for(int n = 0; n < sz; ++n)
            //     v1.push_back(n);
        }
    }

    std::cout << "\nTests for shrink_to_fit():\n";
    {
        cyy::Vector<int> v;
        std::cout << "Default-constructed capacity is " << v.capacity() << '\n';
        // TODO: implemation resize
        // v.resize(100);
        std::cout << "Capacity of a 100-element vector is " << v.capacity() << '\n';
        v.clear();
        std::cout << "Capacity after clear() is " << v.capacity() << '\n';
        v.shrink_to_fit();
        std::cout << "Capacity after shrink_to_fit() is " << v.capacity() << '\n';
    }

    std::cout << "\nTests for assign():\n";
    {
        Vector<char> characters;
        characters.assign(5, 'a');
    
        for (char c : characters) {
            std::cout << c << '\n';
        } 
    }

    std::cout << "\nTests for operator[]\n";
    {
    Vector<int> numbers {2, 4, 6, 8};
 
    std::cout << "Second element: " << numbers[1] << '\n';
 
    numbers[0] = 5;
 
    std::cout << "All numbers:";
    for (auto i : numbers) {
        std::cout << ' ' << i;
    }
    std::cout << '\n';
    }

    std::cout << "\nTests for front and back\n";
    {
        Vector<char> letters {'o', 'm', 'g', 'w', 't', 'f'};
 
        if (!letters.empty()) {
            std::cout << "The first character is: " << letters.front() << '\n';
            std::cout << "The last  character is: " << letters.back()  << '\n';
        }
    }

    std::cout << "\nTests for erase()\n";
    {
        Vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (auto &i : c) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    
        c.erase(c.begin());
    
        for (auto &i : c) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    
        c.erase(c.begin()+2, c.begin()+5);
    
        for (auto &i : c) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    
        // Erase all even numbers (C++11 and later)
        for (auto it = c.begin(); it != c.end(); ) {
            if (*it % 2 == 0) {
                it = c.erase(it);
            } else {
                ++it;
            }
        }
    
        for (auto &i : c) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }

    std::cout << "\nTests for push_back()\n";
    {
        Vector<std::string> numbers;
        std::cout << numbers.size() << " " << numbers.capacity() << std::endl;
 
        numbers.push_back("abc");
        std::cout << numbers.size() << " " << numbers.capacity() << std::endl;
        std::string s = "def";
        numbers.push_back(std::move(s));
        std::cout << numbers.size() << " " << numbers.capacity() << std::endl;
    
        std::cout << "vector holds: ";
        for (auto&& i : numbers) std::cout << std::quoted(i) << ' ';
        std::cout << "\nMoved-from string holds " << std::quoted(s) << '\n';
    }

    std::cout << "\nTests for pop_back()\n";
    {
        Vector<int> numbers;

        numbers.push_back(5);
        for (auto i : numbers)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";

        numbers.push_back(3);
        for (auto i : numbers)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";

        numbers.push_back(4);

        for (auto i : numbers)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
    
        numbers.pop_back();
        for (auto i : numbers)
            std::cout << i << " ";
        std::cout << "\n";
    }
}