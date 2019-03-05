#include "bitset.h"
#include <string>
#include <iostream>
#include <climits>
 
int main() 
{
    using namespace cyy;
    std::cout << "Test for ctors:\n";
    {
        // 空构造函数
        Bitset<8> b1; // [0,0,0,0,0,0,0,0]
    
        // unsigned long long 构造函数
        Bitset<8> b2(42);          // [0,0,1,0,1,0,1,0]
        Bitset<70> bl(ULLONG_MAX); // [0,0,0,0,0,0,1,1,1,...,1,1,1] in C++11
        Bitset<8> bs(0xfff0);      // [1,1,1,1,0,0,0,0]
    
        // string 构造函数
        std::string bit_string = "110010";
        Bitset<8> b3(bit_string);       // [0,0,1,1,0,0,1,0]
        Bitset<8> b4(bit_string, 2);    // [0,0,0,0,0,0,1,0]
        Bitset<8> b5(bit_string, 2, 3); // [0,0,0,0,0,0,0,1]
    
        // 使用自定义零/一数字的 string 构造函数
        std::string alpha_bit_string = "aBaaBBaB";
        Bitset<8> b6(alpha_bit_string, 0, alpha_bit_string.size(),
                        'a', 'B');         // [0,1,0,0,1,1,0,1]
    
        // 使用自定义数字的 char* 构造函数
        Bitset<8> b7("XXXXYYYY", 8, 'X', 'Y'); // [0,0,0,0,1,1,1,1]
    
        Bitset<22> b8("0000000011111111000000");

        std::cout << b1 << '\n' << b2 << '\n' << bl << '\n' << bs << '\n'
                << b3 << '\n' << b4 << '\n' << b5 << '\n' << b6 << '\n'
                << b7 << '\n' << b8 << '\n';
    }

    std::cout << "\nTest for operator[]:\n";
    {
        Bitset<8> b1(42);
        for (std::size_t i = 0; i < b1.size(); ++i) {
            std::cout << "b1["  << i << "]: " << b1[i] << '\n';
        }
        b1[0] = true; // modifies the first bit through bitset::reference
        std::cout << "After setting bit 0, the bitset holds " << b1 << '\n';
        b1[2].flip();
        std::cout << "After flip bit 2, the bitset holds " << b1 << '\n';
    }

    std::cout << "\nTest for operator==:\n";
    {
        Bitset<4> b1(3); // [0,0,1,1]
        Bitset<4> b2(b1);
        Bitset<4> b3(4); // [0,1,0,0]
    
        std::cout << "b1 == b2: " << (b1 == b2) << '\n';  // 1
        std::cout << "b1 == b3: " << (b1 == b3) << '\n';  // 0
        std::cout << "b1 != b3: " << (b1 != b3) << '\n';  // 1
    }

    std::cout << "\nTest for test:\n";
    {
        Bitset<10> b1("1111010000");
    
        size_t idx = 0;
        while (idx < b1.size() && !b1.test(idx)) {
            ++idx;
        }
    
        if (idx < b1.size()) {
            std::cout << "first set bit at index " << idx << '\n';
        } else {
            std::cout << "no set bits\n";
        }
    }

    std::cout << "\nTest for all, any, none:\n";
    {
        Bitset<4> b1("0000");
        Bitset<4> b2("0101");
        Bitset<4> b3("1111");
    
        std::cout << "bitset\t" << "all\t" << "any\t" << "none\n";
        std::cout << b1 << '\t' << b1.all() << '\t' << b1.any() << '\t' << b1.none() << '\n';
        // 0 0 1
        std::cout << b2 << '\t' << b2.all() << '\t' << b2.any() << '\t' << b2.none() << '\n';
        // 0 1 0
        std::cout << b3 << '\t' << b3.all() << '\t' << b3.any() << '\t' << b3.none() << '\n';
        // 1 1 0
    }

    std::cout << "\nTest for count:\n";
    {
        Bitset<8> b("00010010");
        std::cout << "initial value: " << b << '\n';
        // 00010010
    
        // find the first unset bit
        size_t idx = 0;
        while (idx < b.size() && b.test(idx)) ++idx;
    
        // continue setting bits until half the bitset is filled
        while (idx < b.size() && b.count() < b.size()/2) {
            b.set(idx);
            std::cout << "setting bit " << idx << ": " << b << '\n';
            while (idx < b.size() && b.test(idx)) ++idx;
        }
        // 00010011
        // 00010111
    }

    std::cout << "\nTest for set:\n";
    {
        Bitset<8> b;
        for (size_t i = 1; i < b.size(); i += 2) {
            b.set(i);
        }
        std::cout << b << '\n';
        // 10101010
    }

    std::cout << "\nTest for flip:\n";
    {
        Bitset<4> b;

        std::cout << b << "\n";                  // 0000
        std::cout << b.flip(0) << '\n';          // 0001
        std::cout << b.flip(2) << '\n';          // 0101
        std::cout << b.flip() << '\n';           // 1010
    }
}