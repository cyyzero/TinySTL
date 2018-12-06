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
        v.resize(100);
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

    std::cout << "\ntests for swap()\n";
    {
        auto printVector = [] (const Vector<int>& v) {
            for (const auto e : v)
            {
                std::cout << e << " ";
            }
        };
        Vector<int> v1{1, 2, 3};
        Vector<int> v2{7, 8, 9};
    
        std::cout << "v1: ";
        printVector(v1);
    
        std::cout << "\nv2: ";
        printVector(v2);
    
        std::cout << "\n-- SWAP\n";
        v2.swap(v1);
 
        std::cout << "v1: ";
        printVector(v1);
 
        std::cout << "\nv2: ";
        printVector(v2);
        std::cout << "\n";
    }

    std::cout << "\nTest for resize()\n";
    {
        Vector<int> c = {1, 2, 3};
        std::cout << "The vector holds: ";
        for(auto& el: c) std::cout << el << ' ';
        std::cout << '\n';
        c.resize(5);
        std::cout << "After resize up 5: ";
        for(auto& el: c) std::cout << el << ' ';
        std::cout << '\n';
        c.resize(2);
        std::cout << "After resize down to 2: ";
        for(auto& el: c) std::cout << el << ' ';
        std::cout << '\n';
    }

    std::cout << "\nTests for insert()\n";
    {
        Vector<std::string> v{"Helo", "asf", "aaa"};
        std::string l("lvalue test");
        std::string r("rvalue test");
        v.insert(v.begin(), l);
        std::cout << "size: " << v.size() << "\n";
        v.insert(v.begin(), std::move(r));
        std::cout << "size: " << v.size() << "\n";
        
        for (const auto& s: v)
        {
            std::cout << s << std::endl;
        }
        std::cout << "afer move l: " << l << std::endl;
        std::cout << "afer move r: " << r << std::endl;

        auto print_vec = [] (const auto& v) {
            std::cout << "size = " << v.size() << " capacity = " << v.capacity() << "\nelements:   ";
            for (const auto& e : v) {
                std::cout << e << " ";
            }
            std::cout << "\n";
        };

        Vector<int> vec(3,100);
        print_vec(vec);

        auto it = vec.begin();
        it = vec.insert(it, 200);
        print_vec(vec);

        vec.insert(it,2,300);
        print_vec(vec);

        // "it" no longer valid, get a new one:
        it = vec.begin();

        Vector<int> vec2(2,400);
        vec.insert(it+2, vec2.begin(), vec2.end());
        print_vec(vec);

        int arr[] = { 501,502,503 };
        vec.insert(vec.begin(), arr, arr+3);
        print_vec(vec);

        vec.insert(vec.end(), {1,2,3,4});
        print_vec(vec);
    }

    std::cout << "tests for emplack_back()\n";
    {
        struct President
        {
            std::string name;
            std::string country;
            int year;
        
            President(std::string p_name, std::string p_country, int p_year)
                : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
            {
                std::cout << "I am being constructed.\n";
            }
            President(President&& other)
                : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
            {
                std::cout << "I am being moved.\n";
            }
            President& operator=(const President& other) = default;
        };

        Vector<President> elections;
        std::cout << "emplace_back:\n";
        elections.emplace_back("Nelson Mandela", "South Africa", 1994);

        Vector<President> reElections;
        std::cout << "\npush_back:\n";
        reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));
    
        std::cout << "\nContents:\n";
        for (President const& president: elections) {
            std::cout << president.name << " was elected president of "
                    << president.country << " in " << president.year << ".\n";
        }
        for (President const& president: reElections) {
            std::cout << president.name << " was re-elected president of "
                    << president.country << " in " << president.year << ".\n";
        }
    }

    std::cout << "\ntests for operator==/!= ...etc and swap\n";
    {
        Vector<std::string> v1{"abc", "dec", "daf"};
        Vector<std::string> v2 {"fuck"};
        std::cout << std::boolalpha
            << "operator ==: " << (v1 == v2) << "\n"
            << "operator !=: " << (v1 != v2) << "\n"
            << "operator < : " << (v1 <  v2) << "\n"
            << "operator <=: " << (v1 <= v2) << "\n"
            << "operator > : " << (v1 >  v2) << "\n"
            << "operator >=: " << (v1 >= v2) << "\n";

        cyy::swap(v1, v2);
        std::cout << "v1: ";
        for (const auto& e: v1)
            std::cout << e << " ";
        std::cout << "\n";

        std::cout << "v2: ";
        for (const auto& e: v2)
            std::cout << e << " ";
        std::cout << "\n";
    }
}