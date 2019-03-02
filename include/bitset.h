#ifndef BITSET_H
#define BITSET_H

#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace cyy
{

template<std::size_t N>
class Bitset
{
template<typename CharT, typename Traits, std::size_t N_>
friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                                     const Bitset<N_>& x);
public:
    // a proxy object to allow users to interact with individual bits of a Bitset,
    class reference
    {
    public:
        reference& operator=(bool x)
        {
            if (x)
            {
                ref |= (mask & 0xff);
            }
            else
            {
                ref &= (~mask);
            }
            return *this;
        }

        reference& operator=(const reference& x)
        {
            if (x)
            {
                ref |= mask;
            }
            else
            {
                ref &= (~mask);
            }
            return *this;
        }

        // return the referenced bit 
        operator bool() const noexcept
        {
            return static_cast<bool>(ref & mask);
        }

        // return inverted referenced bit 
        bool operator~() const noexcept
        {
            return !static_cast<bool>(ref & mask);
        }

        // inverts the referenced bit
        reference& flip()
        {
            ref ^= mask;
            return *this;
        }

        ~reference() = default;

    private:
        reference(uint8_t& pref, uint8_t pmask)
            : ref(pref), mask(pmask)
        {
        }

        uint8_t& ref;
        uint8_t mask;
    };

    constexpr Bitset()
    {
        std::memset(bits, 0, byte_len);
    }

    constexpr Bitset(unsigned long long val)
    {
        if (sizeof(val) * 8 > N)
        {
            unsigned long long mask = ~((~(unsigned long long)0x0) << N);
            val &= mask;
        }
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
        std::memset(bits, 0, byte_len);
        for (std::size_t i = 0; i < std::min(sizeof(val), byte_len); ++i)
        {
            bits[i] = ptr[i];
        }
    }

    template<typename CharT, typename Traits, typename Alloc>
    explicit Bitset(const std::basic_string<CharT,Traits,Alloc>& str,
                    typename std::basic_string<CharT,Traits,Alloc>::size_type pos = 0,
                    typename std::basic_string<CharT,Traits,Alloc>::size_type n =
                        std::basic_string<CharT,Traits,Alloc>::npos,
                    CharT zero = CharT('0'),
                    CharT one  = CharT('1'))
    {
        if (pos > str.size())
            throw std::out_of_range("pos can't larger than size of str");

        std::size_t bit_len, byte_len;

        bit_len = std::min(n, str.size() - pos);
        bit_len = std::min(bit_len, N);
        byte_len = bit_len/8;

        std::size_t i;
        for (i = 1; i <= byte_len; ++i)
        {
            auto start = pos + bit_len - i * 8;
            uint8_t byte = 0;
            for (std::size_t j = 0; j < 8; ++j, byte <<= 1)
            {
                CharT bit = str[start + j];
                if (Traits::eq(bit, one))
                {
                    byte |= 1;
                }
                else if (!Traits::eq(bit, zero))
                {
                    throw std::invalid_argument("str can't have character other than zero or one");
                }
            }
            bits[i-1] = byte;
        }

        uint8_t byte = 0;
        for (std::size_t j = 0; j < bit_len - byte_len * 8; ++j, byte <<= 1)
        {
            CharT bit = str[pos + j];
            if (Traits::eq(bit, one))
            {
                byte |= 1;
            }
            else if (!Traits::eq(bit, zero))
            {
                throw std::invalid_argument("str can't have character other than zero or one");
            }
        }
        bits[i++] = byte;

        for (; i < byte_len; ++i)
        {
            bits[i] = 0;
        }
    }

    template<typename CharT>
    explicit Bitset(const CharT* str,
                    typename std::basic_string<CharT>::size_type n =
                        std::basic_string<CharT>::npos,
                    CharT zero = CharT('0'),
                    CharT one = CharT('1'))
        : Bitset(n == std::basic_string<CharT>::npos ?
                    std::basic_string<CharT>(str) :
                    std::basic_string<CharT>(str, n),
                 0, n, zero, one)
    {
    }

    ~Bitset() = default;

    bool operator==(const Bitset<N>& rhs) const
    {
        for (std::size_t i = 0; i < byte_len; ++i)
        {
            if (bits[i] != rhs.bits[i])
                return false;
        }
        return true;
    }

    bool operator!=(const Bitset<N>& rhs) const
    {
        return !(*this == rhs);
    }

    bool test(std::size_t pos) const
    {
        if (pos > N)
        {
            throw std::out_of_range("pos can't be larger than N");
        }
        return this->operator[](pos);
    }

    constexpr bool operator[](std::size_t pos) const
    {
        // unlike test(), it does'nt check bound
        uint8_t byte = bits[pos/8];
        uint8_t mask = ((uint8_t)0x1 << (pos % 8));
        return static_cast<bool>(byte & mask);
    }

    reference operator[](std::size_t pos)
    {
        // unlike test(), it does'nt check bound
        uint8_t& byte = bits[pos/8];
        uint8_t mask = ((uint8_t)0x1 << (pos % 8));
        return reference(byte, mask);
    }

    constexpr std::size_t size() const noexcept
    {
        return N;
    }

private:
    static constexpr std::size_t byte_len = (N-1) / 8 + 1;
    uint8_t bits[byte_len];
};

template<typename CharT, typename Traits, std::size_t N>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              const Bitset<N>& x)
{
    // for (std::size_t i = N; i > 0; ++i)
    // {
    //     if (x[i-1])
    //     {
    //         os << '1';
    //     }
    //     else
    //     {
    //         os << '0';
    //     }
    // }

    for (size_t i = 0; i < x.byte_len ; ++i)
    {
        std::cout << (int)x.bits[i] << " ";
    }
    return os;
}

} // namespace cyy

#endif // BITSET_H