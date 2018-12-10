#ifndef UNIQUE_PTR
#define UNIQUE_PTR

#include <type_traits>

namespace cyy
{

namespace
{
// Store pointer and deleter in unique_ptr. Use empty base optimize
template<typename Ptr, typename Del>
struct unique_ptr_data
    : public Del
{
    unique_ptr_data() noexcept = default;

    unique_ptr_data(Ptr p, std::conditional_t<std::is_reference<Del>::value,
                                              Del, const Del&> d) noexcept
        : ptr(p), Del(std::forward<decltype(d)>(d))
    {
    }

    unique_ptr_data(Ptr p, std::remove_reference_t<Del>&& d) noexcept
        : ptr(p), Del(std::forward<decltype(d)>(d))
    {
    }

    unique_ptr_data(unique_ptr_data&& u) noexcept
        : ptr(std::move(u.ptr)), Del(std::move(u.get_del()))
    {
    }

    template<typename U, typename E>
    unique_ptr_data(unique_ptr_data<U, E>&& u) noexcept
        : ptr(std::move(u.ptr)), Del(std::move(u.get_del()))
    {
    }

    ~unique_ptr_data()
    {
        get_del()(ptr);
    }

    Ptr& get_ptr()
    {
        return ptr;
    }

    Del& get_del()
    {
        return static_cast<Del&>(*this);
    }

    private:
        Ptr ptr;
};
} // unnamed namespace

// default deleter for unique_ptr
template<typename T>
struct default_delete
{
    // Constructs a std::default_delete object.
    constexpr default_delete() noexcept = default;

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    default_delete(const default_delete<U>& d) noexcept
    {
    }
    ~default_delete() = default;

    void operator()(T* ptr) const
    {
        static_assert(sizeof(T) > 0, "Can't delete a pointer to an incomplete type");
        delete ptr;
    }

    template<typename U>
    void operator()(U* ptr) const
    {
        static_assert(sizeof(U) > 0, "Can't delete a pointer to an incomplete type");
        delete ptr;
    }
};

template<typename T>
struct default_delete<T[]>
{

};

template<typename T, typename Deleter = cyy::default_delete<T>>
class unique_ptr
{
    struct pointer_helper
    {
        template<typename U>
        static typename U::pointer
        test(typename U::pointer*);

        template<typename>
        static T* test(...);

        using type = decltype(test<std::remove_reference_t<Deleter>>(nullptr));
    };
public:

    static_assert(std::is_rvalue_reference<Deleter>::value,
                  "unique_ptr can't be initialized by a Deleter which is a rvalue reference");

    using pointer      = typename pointer_helper::type;
    using element_type = T;
    using deleter_type = Deleter;

    // constructor

    constexpr
    unique_ptr() noexcept
        : data()
    {
    }

    constexpr
    unique_ptr(std::nullptr_t) noexcept
        : data(nullptr)
    {
    }

    explicit
    unique_ptr(pointer p) noexcept
        : data(p)
    {
        static_assert(std::is_pointer<deleter_type>::value,
            "unique_ptr(pointer p) is ill-formed if Deleter is of pointer or reference type");
    }

    unique_ptr(pointer p, std::conditional_t<std::is_reference<deleter_type>::value, 
                                             deleter_type, 
                                             const deleter_type&> d) noexcept
        : data(p, std::forward<decltype(d)>(d))
    {
    }

    unique_ptr(pointer p, std::remove_reference_t<deleter_type>&& d)
        : data(p, std::forward<dectltype(d)>(d))
    {
        static_assert(std::is_reference<deleter_type>::value,
                      "rvalue bound to reference");
    }

    // TODO: check
    unique_ptr(unique_ptr&& u) noexcept
        : data(std::move(u.data))
    {
    }

    template<typename U, typename E, typename = std::enable_if_t<
        std::is_convertible<typename unique_ptr<U, E>::pointer,
                            pointer>::value &&
        !std::is_array<U>::value &&
        std::conditional_t<std::is_reference<deleter_type>::value,
                           std::is_same<E, deleter_type>,
                           std::is_convertible<E, deleter_type>>::value>>
    unique_ptr(unique_ptr<U, E>&& u) noexcept
        : data(std::move(u.data))
    {
    }

    // destructor
    ~unique_ptr() = default;

    // Note: cyy::unique_ptr doesn't support construct from auto_ptr
    // actually cyy::auto_ptr doesn't exits



private:
    unique_ptr_data<pointer, deleter_type> data;

};

} // namespce cyy

#endif // UNIQUE_PTR
