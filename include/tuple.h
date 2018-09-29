#ifndef TUPLE_H
#define TUPLE_H

#include <type_traits>
#include <utility>
#include <memory>
#include "pair.h"

namespace cyyzero
{
namespace
{

template<std::size_t Index, typename Head, bool IsEmpty>
struct Head_base;

template<std::size_t Index, typename Head>
struct Head_base<Index, Head, true>: public Head
{
    constexpr Head_base()
        : Head() { }

    constexpr Head_base(const Head& head)
        : Head(head) { }

    template<typename UHead>
    Head_base(UHead&& head)
        : Head(std::forward<UHead>(h)) { }

    Head& head()
    {
        return *this;
    }

    const Head& head() const
    {
        return *this;
    }

    void swap_impl(Head& head)
    {
        using std::swap;
        swap(h, head());
    }
};

template<std::size_t Index, typename... Elements>
struct Tuple_impl;

// 0-element Tuple_impl. Basic case for the inheritance recursion.
template<std::size_t Index>
struct Tuple_impl<Index>
{
protected:
    void swap_impl(Tuple_impl&) { }
};

template<std::size_t Index, typename Head, typename... Tail>
struct Tuple_impl<Index, Head, Tail...>
    : public Tuple_impl<Index+1, Tail...>,
      private Head_base<Index, Head, std::is_empty_v<Head>>
{
    using Inherited = Tuple_impl<Index+1, Tail...>;
    using Base      = Head_base<Index, Head, std::is_empty_v<Head>>;

    Head& head()
    {
        return Base::head();
    }

    const Head& head() const
    {
        return Base::head();
    }

    Inherited& tail()
    {
        return *this;
    }

    const Inherited& tail() const
    {
        return *this;
    }

    // constructors
    constexpr Tuple_impl()
        : Inherited(), Base() { }

    explicit constexpr Tuple_impl(const Head& head, const Tail&... tail)
        : Inherited(tail...), Base(head) { }

    template<typename UHead, typename... UTail>
    explicit Tuple_impl(UHead&& head, UTail&&... tail)
        : Inherited(std::forward<UTail>(tail)...),
          Base(std::forward<UHead>(head)) { }

    constexpr Tuple_impl(const Tuple_impl&) = default;

    Tuple_impl(Tuple_impl&& other)
        : Inherited(std::move(other.tail())),
          Base(std::forward<Head>(other.head())) { }

    template<typename... UElements>
    Tuple_impl(const Tuple_impl<Index, UElements...>& other)
        : Inherited(other.tail()), Base(other.head()) { }

    template<typename UHead, typename... UTail>
    Tuple_impl(Tuple_impl<Index, UHead, UTail...>&& other)
        : Inherited(std::move(other.tail())), Base(std::forward<UHead>(other.head)) { }

    // assignments
    Tuple_impl& operator=(const Tuple_impl& other)
    {
        head() = other.head();
        tail() = other.tail();
        return *this;
    }

    Tuple_impl& operator=(Tuple_impl&& other)
    {
        head() = std::forward<Head>(other.head());
        tail() = std::move(other.tail());
        return *this;
    }

    template<typename... UElements>
    Tuple_impl& operator=(const Tuple_impl<Index, UElements...>& other)
    {
        head() = other.head();
        tail() = other.head();
        return *this;
    }

    template<typename UHead, typename... UTail>
    Tuple_impl& operator=(Tuple_impl<Index, UHead, UTail...>&& other)
    {
        head() = std::forward<UHead>(other.head());
        tail() = std::move(other.tail());
        return *this;
    }

protected:
    void swap_impl(Tuple_impl& other)
    {
        Base::swap_impl(other.head());
        Inherited::swap_impl(other.tail());
    }
};
} // unnamed namespace

template<typename... Elements>
class Tuple : public Tuple_impl<0, Elements...>
{
    using Inherited = Tuple_impl<0, Elements...>;
public:
    // constructors
    constexpr Tuple()
        : Inherited() { }

    explicit constexpr Tuple(const Elements&... elements)
        : Inherited(elements...) { }

    template<typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    explicit explicit Tuple(UElements&&... elements)
        : Inherited(std::forward<UElements>(elements)...) { }

    template<typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    constexpr Tuple(const Tuple<UElements...>& other)
        : Inherited(static_cast<Tuple_impl<0, UElements...>&>(other)) { }

    template<typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    constexpr Tuple(Tuple<UElements...>&& other)
        : Inherited(static_cast<Tuple_impl<0, UElements...>&&>(other)) { }

    Tuple(const Tuple&) = default;

    Tuple(Tuple&&) = default;

    template<typename Alloc>
    Tuple(std::allocator_arg_t tag, const Alloc& a)
        : Inherited(tag, a) { }

    template<typename Alloc>
    Tuple(std::allocator_arg_t tag, const Alloc& a, const Elements&... elements)
        : Inherited(tag, a, elements...) { }

    template<typename Alloc, typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    Tuple(std::allocator_arg_t tag, const Alloc& a, UElements&&... elements)
        : Inherited(tag, a, std::forward<UElements>(elements)...) { }

    template<typename Alloc, typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    Tuple(std::allocator_arg_t tag, const Alloc& a, const Tuple<UElements...>& other)
        : Inherited(tag, a, static_cast<Tuple_impl<0, UElements...>&>(other)) { }

    template<typename Alloc, typename... UElements, typename = std::enable_if_t<
             std::conjunction_v<std::is_convertible<UElements, Elements>...>>>
    Tuple(std::allocator_arg_t tag, const Alloc& a, Tuple<UElements...>&& other)
        : Inherited(tag, a, static_cast<Tuple_impl<0, UElements...>&&>(other)) { }

    template<typename Alloc>
    Tuple(std::allocator_arg_t tag, const Alloc& a, const Tuple& other)
        : Inherited(tag, a, static_cast<const Inherited&>(other)) { }

    template<typename Alloc>
    Tuple(std::allocator_arg_t tag, const Alloc& a, Tuple&& other)
        : Inherited(tag, a, static_cast<Inherited&&>(other)) { }


    // assignment
    Tuple& operator=(const Tuple& other)
    {
        static_cast<Inherited&>(*this) = other;
        return *this;
    }

    Tuple& operator=(Tuple&& other)
    {
        static_cast<Inherited&>(*this) = std::move(other);
        return *this;
    }

    template<typename... UElements, typename = std::enable_if_t<
             sizeof...(UElements) == sizeof...(Elements)>>
    Tuple& operator=(const Tuple<UElements...>& other)
    {
        static_cast<Inherited&>(*this) = other;
        return *this;
    }

    template<typename... UElements, typename = std::enable_if_t<
             sizeof...(UElements) == sizeof...(Elements)>>
    Tuple& operator=(Tuple<UElements...>&& other)
    {
        static_cast<Inherited&>(*this) = std::move(other);
        return *this;
    }

    void swap(Tuple& other)
    {
        Inherited::swap_impl(other);
    }
};

// Specialization for 0-element tuple
template<>
class Tuple<>
{
    void swap(Tuple& other)
    {
    }
};
// Specialization for 2-elements tuple
template<typename T1, typename T2>
class Tuple<T1, T2> : public Tuple_impl<0, T1, T2>
{
    using Inherited = Tuple_impl<0, T1, T2>;

public:

    // constructors

    constexpr Tuple()
        : Inherited() {}

    explicit constexpr Tuple(const T1& a1, const T2& a2)
        : Inherited(a1, a2) { }

    template<typename U1, typename U2>
    explicit Tuple(U1&& a1, U2&& a2)
        : Inherited(std::forward<U1>(a1), std::forward<U2>(a2)) { }

    constexpr Tuple(const Tuple&) = default;

    Tuple(Tuple&& other)
        : Inherited(static_cast<Inherited&&>(other)) { }

    template<typename U1, typename U2>
    Tuple(const Tuple<U1, U2>& other)
        : Inherited(static_cast<Tuple_impl<0, U1, U2>&>(other)) { }

    template<typename U1, typename U2>
    Tuple(Tuple<U1, U2>&& other)
        : Inherited(static_cast<Tuple_impl<0, U1, U2>&&>(other)) { }

    template<typename U1, typename U2>
    Tuple(const pair<U1, U2>& other)
        : Inherited(other.first, other.second) { }

    template<typename U1, typename U2>
    Tuple(pair<U1, U2>&& other)
        : Inherited(std::forward<U1>(other.first), std::forward<U2>(other.second)) { }

    // assignments

    Tuple& operator=(const Tuple& other)
    {
        static_cast<Inherited&>(*this)= (other);
        return *this;
    }

    Tuple& operator=(Tuple&& other)
    {
        static_cast<Inherited&>(*this) = std::move(other);
        return *this;
    }

    template<typename U1, typename U2>
    Tuple& operator=(const Tuple<U1, U2>& other)
    {
        static_cast<Tuple_impl<0, U1, U2>&>(*this) = other;
        return *this;
    }

    template<typename U1, typename U2>
    Tuple& operator=(Tuple<U1, U2>&& other)
    {
        static_cast<Tuple_impl<0, U1, U2>&>(*this) = std::move(other);
        return *this;
    }

    void swap(Tuple& other)
    {
        using std::swap;
        swap(this->head(), other.head());
        swap(this->tail().head(), other.tail().head());
    }
};

// Specialization for 1-element tuple
template<typename T>
class Tuple<T> : public Tuple_impl<0, T>
{
    using Inherited = Tuple_impl<0, T>;

public:
    // constructors
    constexpr Tuple()
        : Inherited() { }

    explicit constexpr Tuple(const T& a)
        : Inherited(a) { }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    explicit Tuple(U&& a)
        : Inherited(std::forward<U>(a)) { }

    constexpr Tuple(const Tuple&) = default;

    Tuple(Tuple&& other)
        : Inherited(static_cast<Inherited&&>(other)) { }

    template<typename U>
    Tuple(const Tuple<U>& other)
        : Inherited(static_cast<Tuple_impl<0, U>&>(other)) { }

    template<typename U>
    Tuple(Tuple<U>&& other)
        : Inherited(static_cast<Tuple_impl<0, U>&&>(other)) { }

    // assignments
    Tuple& operator=(const Tuple& other)
    {
        static_cast<Inherited&>(*this) = other;
        return *this;
    }

    Tuple& operator=(Tuple&& other)
    {
        static_cast<Inherited&>(*this) = std::move(other);
        return *this;
    }

    template<typename U>
    Tuple& operator=(const Tuple<U>& other)
    {
        static_cast<Inherited&>(*this) = other;
        return *this;
    }

    template<typename U>
    Tuple& operator=(Tuple<U>&& other)
    {
        static_cast<Inherited&>(*this) = std::move(other);
        return *this; 
    }

    void swap(Tuple& other)
    {
        Inherited::swap_impl(other);
    }
};

} // namespace cyyzero

#endif // TUPLE_H