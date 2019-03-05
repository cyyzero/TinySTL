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
    friend class Bitset<N>;
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

    // constructors
    constexpr Bitset()
    {
        std::memset(bits, 0, BYTE_LEN);
    }

    constexpr Bitset(unsigned long long val)
    {
        if constexpr (sizeof(val) * 8 > N)
        {
            constexpr unsigned long long mask = ~((~(unsigned long long)0x0) << N);
            val &= mask;
        }
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
        std::size_t len = std::min(sizeof(val), BYTE_LEN);
        for (std::size_t i = 0; i < len; ++i)
        {
            bits[i] = ptr[i];
        }
        for (std::size_t i = len; i < BYTE_LEN; ++i)
        {
            bits[i] = 0;
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
        byte_len = bit_len / 8;

        std::size_t i;
        for (i = 1; i <= byte_len; ++i)
        {
            auto start = pos + bit_len - i * 8;
            uint8_t byte = 0;
            for (std::size_t j = 0; j < 8; ++j)
            {
                byte <<= 1;
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

        if (bit_len % 8)
        {
            uint8_t byte = 0;
            for (std::size_t j = 0; j < bit_len - byte_len * 8; ++j)
            {
                byte <<= 1;
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
            bits[i-1] = byte;
        }

        for (; i < BYTE_LEN; ++i)
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

    // destructor
    ~Bitset() = default;

    // compare the contents
    bool operator==(const Bitset<N>& rhs) const
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
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

    // access specific bit
    bool test(std::size_t pos) const
    {
        if (pos > N)
        {
            throw std::out_of_range("pos can't be larger than N");
        }
        return this->operator[](pos);
    }

    // access specific bit
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

    // check if all, any or none of the bits are set to true
    bool all() const
    {
        return check_bytes(0xff);
    }

    bool none() const
    {
        return check_bytes(0);
    }

    bool any() const
    {
        return !check_bytes(0);
    }

    // count the number of bit that is true
    std::size_t count() const
    {
        std::size_t count = 0;

        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            count += count_byte(bits[i]);
        }

        return count;
    }


    // set all bits to true
    Bitset& set()
    {
        for (size_t i = 0; i < BYTE_LEN-1; ++i)
            bits[i] = 0xff;

        bits[BYTE_LEN-1] = 0xff << (N % 8);
        return *this;
    }

    // get size
    constexpr std::size_t size() const noexcept
    {
        return N;
    }

    // Bitset& operator&=(const Bitset& other)
    // {
    //     for (std::size_t i = 0; i < BYTE_LEN; ++i)
    //     {
    //         bits[i] &= other.bits[i];
    //     }

    //     // // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.

    //     return *this;
    // }

    // Bitset& operator|=(const Bitset& other)
    // {
    //     for (std::size_t i = 0; i < BYTE_LEN; ++i)
    //     {
    //         bits[i] |= other.bits[i];
    //     }

    //     // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.

    //     return *this;
    // }

    // Bitset& operator^=(const Bitset& other)
    // {
    //     for (std::size_t i = 0; i < BYTE_LEN; ++i)
    //     {
    //         bits[i] ^= other.bits[i];
    //     }

    //     // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.

    //     return *this;
    // }

    // Bitset operator~()
    // {
    //     return Bitset(*this).flip();
    // }

    // set the bit at position pos to the value value.
    Bitset& set(std::size_t pos, bool value = true)
    {
        uint8_t mask = 0x1 << (pos % 8);
        if (value)
            bits[pos/8] |= mask;
        else
            bits[pos/8] &= ~mask;
        return *this;
    }

    // flip bits
    Bitset& flip()
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            bits[i] = ~bits[i];
        }

        // ~0 = 1, so have to set the left (N%8) bits to zero
        if constexpr (N % 8 != 0)
        {
            constexpr uint8_t mask = (uint8_t)~(0xff << (N % 8));
            bits[BYTE_LEN-1] &= mask;
        }

        return *this;
    }

    Bitset& flip(std::size_t pos)
    {
        if (pos >= N)
            throw std::out_of_range("pos can't be larger than N.");

        uint8_t mask = (0x1 << (pos % 8));
        bits[pos / 8] ^= mask;

        return *this;
    }

private:

    bool check_bytes(uint8_t num) const
    {
        constexpr std::size_t byte_len = N / 8;
        constexpr uint8_t left = N % 8;
        for (std::size_t i = 0; i < byte_len; ++i)
        {
            if (num != bits[i])
                return false;
        }
        if constexpr (left != 0)
        {
            constexpr uint8_t mask = (uint8_t)~(0xff << left);
            if ((bits[byte_len] & mask) != (num & mask))
                return false;
        }
        return true;
    }

    std::size_t count_byte(uint8_t num) const
    {
        std::size_t cnt = 0;
        while (num)
        {
            num &= (num-1);
            ++cnt;
        }
        return cnt;
    }

    static constexpr std::size_t BYTE_LEN = (N-1) / 8 + 1;
    uint8_t bits[BYTE_LEN];
};

template<typename CharT, typename Traits, std::size_t N>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              const Bitset<N>& x)
{
    for (std::size_t i = N; i > 0; --i)
    {
        if (x[i-1])
        {
            os << '1';
        }
        else
        {
            os << '0';
        }
    }
    return os;
}

} // namespace cyy

#endif // BITSET_H