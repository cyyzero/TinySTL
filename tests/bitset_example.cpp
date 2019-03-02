#include "bitset.h"
#include <string>
#include <iostream>
#include <climits>
 
int main() 
{
    using namespace cyy;
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
 
    std::cout << b1 << '\n' << b2 << '\n' << bl << '\n' << bs << '\n'
              << b3 << '\n' << b4 << '\n' << b5 << '\n' << b6 << '\n'
              << b7 << '\n';
}