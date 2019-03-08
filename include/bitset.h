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
friend std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const Bitset<N_>& x);

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

    // get size
    constexpr std::size_t size() const noexcept
    {
        return N;
    }

    // perform binary AND, OR, XOR and NOT 
    Bitset& operator&=(const Bitset& other)
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            bits[i] &= other.bits[i];
        }
        // // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.
        return *this;
    }

    Bitset& operator|=(const Bitset& other)
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            bits[i] |= other.bits[i];
        }
        // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.
        return *this;
    }

    Bitset& operator^=(const Bitset& other)
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            bits[i] ^= other.bits[i];
        }
        // 0 ^ 0 = 0, so don't have to set the left (N%8) bits.
        return *this;
    }

    Bitset operator~()
    {
        return Bitset(*this).flip();
    }

    // perform binary shift left and shift right
    Bitset operator<<(std::size_t pos) const
    {
        return (Bitset(*this) <<= pos);
    }

    Bitset& operator<<=(std::size_t pos)
    {
        if (pos >= N)
        {
            set();
        }

        uint8_t left_bits = pos % 8;
        std::size_t gap = pos / 8;

        for (int i = (int)BYTE_LEN - 1; i > (int)gap; --i)
        {
            bits[i] = (bits[i-gap] << left_bits) + (bits[i-gap-1] >> (8-left_bits));
        }
        bits[gap] = bits[0] << left_bits;
        for (int i = (int)gap-1; i >= 0; ++i)
        {
            bits[i] = 0;
        }

        if constexpr (N % 8 != 0)
        {
            set_left_bits();
        }

        return *this;
    }

    Bitset operator>>(std::size_t pos) const
    {
        return (Bitset(*this) >>= pos);
    }

    Bitset& operator>>=(std::size_t pos)
    {
        if (pos >= N)
        {
            set();
        }

        uint8_t left_bits = pos % 8;
        std::size_t gap = pos / 8;

        for (std::size_t i = 0; i < BYTE_LEN - gap - 1; ++i)
        {
            bits[i] = (bits[i+gap] >> left_bits) + (bits[i+gap+1] << (8-left_bits));
        }
        bits[BYTE_LEN - gap - 1] = (bits[BYTE_LEN - 1] >> left_bits);
        for (std::size_t i = BYTE_LEN - gap; i < BYTE_LEN; ++i)
        {
            bits[i] = 0;
        }

        return *this;
    }

    // set all bits to true
    Bitset& set()
    {
        for (size_t i = 0; i < BYTE_LEN-1; ++i)
            bits[i] = 0xff;

        bits[BYTE_LEN-1] = 0xff << (N % 8);
        return *this;
    }

    // set the bit at position pos to the value.
    Bitset& set(std::size_t pos, bool value = true)
    {
        uint8_t mask = 0x1 << (pos % 8);
        if (value)
            bits[pos/8] |= mask;
        else
            bits[pos/8] &= ~mask;
        return *this;
    }

    // Sets all bits to false
    Bitset& reset()
    {
        for (std::size_t i = 0; i < BYTE_LEN; ++i)
        {
            bits[i] = 0;
        }
        return *this;
    }

    Bitset& reset(std::size_t pos)
    {
        if (pos > N)
        {
            throw std::out_of_range("pos can't be larger than N");
        }

        uint8_t mask = 0x1 << (pos % 8);
        bits[pos / 8] &= ~mask;

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
            set_left_bits();
        }

        return *this;
    }

    Bitset& flip(std::size_t pos)
    {
        if (pos >= N)
        {
            throw std::out_of_range("pos can't be larger than N");
        }

        uint8_t mask = 0x1 << (pos % 8);
        bits[pos / 8] ^= mask;

        return *this;
    }

    // convert the contents to a string
    template<typename CharT, typename Traits = std::char_traits<CharT>, typename Allocator = std::allocator<CharT>>
    std::basic_string<CharT, Traits, Allocator> to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const
    {
        std::basic_string<CharT, Traits, Allocator> ans;
        for (std::size_t i = N; i > 0; --i)
        {
            if (operator[](i-1))
            {
                ans.push_back(one);
            }
            else
            {
                ans.push_back(zero);
            }
        }
        return ans;
    }

    auto to_string() const
    {
        return to_string<char, std::char_traits<char>, std::allocator<char>>();
    }

    // convert the contents to unsigned long
    unsigned long to_ulong() const
    {
        return to_uint_t<unsigned long>("can't convert to unsigned long");
    }

    // convert the contents to unsigned long long
    unsigned long long to_ullong() const
    {
        return to_uint_t<unsigned long long>("can't convert to unsigned long long");
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

    void set_left_bits()
    {
        constexpr uint8_t mask = (uint8_t)~(0xff << (N % 8));
        bits[BYTE_LEN-1] &= mask;
    }

    // convert to uint_t type unsigned integer
    template<typename uint_t>
    uint_t to_uint_t(const char* except) const
    {
        if (BYTE_LEN > sizeof(uint_t))
            throw std::overflow_error(except);

        uint_t ans = 0;

        for (std::size_t i = BYTE_LEN; i > 0; --i)
        {
            ans <<= 8;
            ans += bits[i-1];
        }

        return ans;
    }

    static constexpr std::size_t BYTE_LEN = (N-1) / 8 + 1;
    uint8_t bits[BYTE_LEN];
};

// perform binary logic operations on bitsets
template<std::size_t N>
Bitset<N> operator&(const Bitset<N>& lhs, const Bitset<N>& rhs) noexcept
{
    return Bitset<N>(lhs) &= rhs;
}

template<std::size_t N>
Bitset<N> operator|(const Bitset<N>& lhs, const Bitset<N>& rhs) noexcept
{
    return Bitset<N>(lhs) |= rhs;
}

template<std::size_t N>
Bitset<N> operator^(const Bitset<N>& lhs, const Bitset<N>& rhs) noexcept
{
    return Bitset<N>(lhs) ^= rhs;
}

// perform stream input and output of bitsets 
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

template<typename CharT, typename Traits, std::size_t N>
std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is,
                                              Bitset<N>& x)
{
    char buf[N+1] = { };
    is.read(buf, N);
    x = Bitset<N>(buf);
    return is;
}

} // namespace cyy

#endif // BITSET_H