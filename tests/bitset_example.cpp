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

    std::cout << "\nTest for operator[]\n";
    {
    Bitset<8> b1(42);
    for (std::size_t i = 0; i < b1.size(); ++i) {
        std::cout << "b1[" << i << "]: " << b1[i] << '\n';
    }
    b1[0] = true; // modifies the first bit through bitset::reference
    std::cout << "After setting bit 0, the bitset holds " << b1 << '\n';
    }
}