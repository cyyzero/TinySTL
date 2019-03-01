#ifndef BITSET_H
#define BITSET_H

#include <string>
#include <cstring>

namespace cyy
{

template<std::size_t N>
class Bitset
{
public:
    // a proxy object to allow users to interact with individual bits of a bitset,
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
        alloc();
        init();
    }

    constexpr Bitset(unsigned long long val)
    {
        alloc();

    }

    template<typename CharT, typename Traits, typename Alloc>
    explicit Bitset(const std::basic_string<CharT, Traits, Alloc>& str,
                    typename std::basic_string<CharT, Traits, Alloc>::size_type pos = 0,
                    typename std::basic_string<CharT, Traits, Alloc>::size_type n = 
                        std::basic_string<CharT, Traits, Alloc>::npos)
    {

    }

    template<typename CharT, typename Traits, typename Alloc>
    explicit bitset(const std::basic_string<CharT,Traits,Alloc>& str,
                    typename std::basic_string<CharT,Traits,Alloc>::size_type pos = 0,
                    typename std::basic_string<CharT,Traits,Alloc>::size_type n =
                        std::basic_string<CharT,Traits,Alloc>::npos,
                    CharT zero = CharT('0'),
                    CharT one = CharT('1'))
    {
        
    }

    ~Bitset()
    {
        delete []bits;
    }

    bool operator==(const Bitset<N>& rhs) const
    {

    }

    bool operator!=(const Bitset<N>& rhs) const
    {
        return !(*this == rhs);
    }

    constexpr bool operator[](std::size_t pos) const
    {
        
    }

    reference operator[](std::size_t pos)
    {

    }

    constexpr std::size_t size() const noexcept
    {
        return N;
    }

private:

    void alloc()
    {
        byte_len = N / 8 + 1;
        bits = new uint8_t[byte_len];
    }

    void init()
    {
        std::memset((void*)bits, 0, byte_len);
    }

    uint8_t* bits;
    std::size_t byte_len;
};

} // namespace cyy

#endif // BITSET_H