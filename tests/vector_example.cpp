#include <string>
#include <memory>
#include <vector>
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
              << " nums2: " << nums2.size()
              << " nums3: " << nums3.size() << '\n';
}
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
}