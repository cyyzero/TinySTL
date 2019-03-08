#include "bitset.h"
#include <string>
#include <iostream>
#include <sstream>
#include <climits>
#include <cassert>
#include <limits>
 
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
    
        assert((b1 == b2) == 1);
        std::cout << "b1 == b2: " << (b1 == b2) << '\n';  // 1
        assert((b1 == b3) == 0);
        std::cout << "b1 == b3: " << (b1 == b3) << '\n';  // 0
        assert((b1 != b3) == 1);
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
        assert(b1.all() == 0);
        assert(b1.any() == 0);
        assert(b1.none() == 1);
        std::cout << b1 << '\t' << b1.all() << '\t' << b1.any() << '\t' << b1.none() << '\n';
        // 0 0 1

        assert(b2.all() == 0);
        assert(b2.any() == 1);
        assert(b2.none() == 0);
        std::cout << b2 << '\t' << b2.all() << '\t' << b2.any() << '\t' << b2.none() << '\n';
        // 0 1 0

        assert(b3.all() == 1);
        assert(b3.any() == 1);
        assert(b3.none() == 0);
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

    std::cout << "\nTest for flip:\n";
    {
        Bitset<4> b;

        std::cout << b << "\n";                  // 0000
        assert(b == Bitset<4>("0000"));
        std::cout << b.flip(0) << '\n';          // 0001
        assert(b == Bitset<4>("0001"));
        std::cout << b.flip(2) << '\n';          // 0101
        assert(b == Bitset<4>("0101"));
        std::cout << b.flip() << '\n';           // 1010
        assert(b == Bitset<4>("1010"));
    }

    std::cout << "\nTest for operator&=,|=,^=,~:\n";
    {
        Bitset<16> dest;
        std::string pattern_str = "1001";
        Bitset<16> pattern(pattern_str);
    
        for (size_t i = 0, ie = dest.size()/pattern_str.size(); i != ie; ++i) {
            dest <<= pattern_str.size();
            dest |= pattern;
        }
        assert(dest == Bitset<16>("1001100110011001"));
        std::cout << dest << '\n';
    }

    std::cout << "\nTest for operator<<,<<=,>>,>>=:\n";
    {
        Bitset<8> b("01110010");
        std::cout << "initial value: " << b << '\n';
    
        while (b.any()) {
            while (!b.test(0)) {
                b >>= 1;
            }
            std::cout << b << '\n';
            b >>= 1;
        }
        // 00111001
        // 00000111
        // 00000011
        // 00000001
    }

    std::cout << "\nTest for set:\n";
    {
        Bitset<8> b;
        for (size_t i = 1; i < b.size(); i += 2) {
            b.set(i);
        }
        assert(b == decltype(b)("10101010"));
        std::cout << b << '\n';
        // 10101010
    }

    std::cout << "\nTest for reset:\n";
    {
        Bitset<8> b(42);
        assert(b == Bitset<8>("00101010"));
        std::cout << "Bitset is         " << b << '\n';

        b.reset(1);
        assert(b == Bitset<8>("00101000"));
        std::cout << "After b.reset(1): " << b << '\n';

        b.reset();
        assert(b == Bitset<8>());
        std::cout << "After b.reset():  " << b << '\n';
    }

    std::cout << "\nTest for to_string:\n";
    {
        Bitset<8> b(42);
        assert (b.to_string() == "00101010");
        std::cout << b.to_string() << '\n';
        assert(b.to_string('*') == "**1*1*1*");
        std::cout << b.to_string('*') << '\n';
        assert(b.to_string('O', 'X') == "OOXOXOXO");
        std::cout << b.to_string('O', 'X') << '\n';
    }

    std::cout << "\nTest for to_ulong:\n";
    {
        for (unsigned long i = 0; i < 10; ++i) {
            Bitset<5> b(i);
            Bitset<5> b_inverted = ~b;
            std::cout << i << '\t';
            std::cout << b << '\t';
            std::cout << b_inverted << '\t';
            std::cout << b_inverted.to_ulong() << '\n'; 
        }
    }

    std::cout << "\nTest for to_ullong:\n";
    {
        Bitset<std::numeric_limits<unsigned long long>::digits> b(
            0x123456789abcdef0LL
        );

        std::cout << b << "  " << std::hex << b.to_ullong() << '\n';
        assert(b == decltype(b)("0001001000110100010101100111100010011010101111001101111011110000") &&
               b.to_ullong() == 0x123456789abcdef0);
        // 0001001000110100010101100111100010011010101111001101111011110000
        // 0001001000110100010101100111100010011010101111001101111011110000
        b.flip();
        assert(b == decltype(b)("1110110111001011101010011000011101100101010000110010000100001111") &&
               b.to_ullong() == 0xedcba9876543210f);
        std::cout << b << "  " << b.to_ullong() << '\n';
    }

    std::cout << "\nTest for operator&,|,^:\n";
    {
        Bitset<4> b1("0110");
        Bitset<4> b2("0011");
        assert((b1 & b2) == Bitset<4>("0010"));
        std::cout << "b1 & b2: " << (b1 & b2) << '\n';
        assert((b1 | b2) == Bitset<4>("0111"));
        std::cout << "b1 | b2: " << (b1 | b2) << '\n';
        assert((b1 ^ b2) == Bitset<4>("0101"));
        std::cout << "b1 ^ b2: " << (b1 ^ b2) << '\n';
    }

    std::cout << "\nTest for operator <<,>>:\n";
    {
        std::string bit_string = "001101";
        std::istringstream bit_stream(bit_string);
    
        Bitset<3> b1;
        bit_stream >> b1; // reads "001", stream still holds "101"
        assert(b1.to_string() == "001");
        std::cout << b1 << '\n';
    
        Bitset<8> b2;
        bit_stream >> b2; // reads "101", populates the 8-bit set as "00000101"
        std::cout << b2 << '\n';
        assert(b2.to_string() == "00000101");
    }
}