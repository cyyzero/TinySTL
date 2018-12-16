#ifndef UNIQUE_PTR
#define UNIQUE_PTR

#include <type_traits>

namespace cyy
{

namespace detail
{

template<typename T>
struct is_reference_or_pointer_or_final
    : public std::disjunction<std::is_reference<T>,
                              std::is_pointer<T>,
                              std::is_final<T>>
{
};

// Store pointer and deleter in Unique_ptr. Use empty base optimize
template<typename Ptr, typename Del, bool IsReferenceOrPointerOrFinal = 
         is_reference_or_pointer_or_final<Del>::value>
struct Unique_ptr_data;

template<typename Ptr, typename Del>
struct Unique_ptr_data<Ptr, Del, true>
{
    Unique_ptr_data() noexcept = default;

    Unique_ptr_data(Ptr p) noexcept
        : ptr(p), del()
    {
    }

    template<typename D>
    Unique_ptr_data(Ptr p, D&& d) noexcept
        : ptr(p), del(std::forward<D>(d))
    {
    }

    ~Unique_ptr_data() = default;

    Ptr& get_ptr()
    {
        return ptr;
    }

    const Ptr& get_ptr() const
    {
        return ptr;
    }

    Del& get_del()
    {
        return del;
    }

    const Del& get_del() const
    {
        return del;
    }

    void swap(Unique_ptr_data& data) noexcept
    {
        using std::swap;
        swap(ptr, data.ptr);
        swap(del, data.del);
    }

    private:
        Ptr ptr;
        Del del;
};

template<typename Ptr, typename Del>
struct Unique_ptr_data<Ptr, Del, false>
    : public Del
{
    Unique_ptr_data() noexcept
        : Del(), ptr()
    {
    }

    Unique_ptr_data(Ptr p) noexcept
        : Del(), ptr(p)
    {
    }

    template<typename D>
    Unique_ptr_data(Ptr p, D&& d) noexcept
        : Del(std::forward<D>(d)), ptr(p)
    {
    }

    ~Unique_ptr_data() = default;

    Ptr& get_ptr()
    {
        return ptr;
    }

    const Ptr& get_ptr() const
    {
        return ptr;
    }

    Del& get_del()
    {
        return static_cast<Del&>(*this);
    }

    const Del& get_del() const
    {
        return static_cast<const Del&>(*this);
    }

    void swap(Unique_ptr_data& data) noexcept
    {
        using std::swap;
        swap(ptr, data.ptr);
        swap(get_del(), data.get_del());
    }

    private:
        Ptr ptr;
};

} // namespace detail

// default deleter for Unique_ptr
template<typename T>
struct Default_delete
{
    // Constructs a std::default_delete object.
    constexpr Default_delete() noexcept = default;

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    Default_delete(const Default_delete<U>& d) noexcept
    {
    }

    ~Default_delete() = default;

    void operator()(T* ptr) const
    {
        static_assert(sizeof(T) > 0,
                      "Can't delete a pointer to an incomplete type");
        delete ptr;
    }

    template<typename U>
    void operator()(U* ptr) const = delete;
};

template<typename T>
struct Default_delete<T[]>
{
    constexpr Default_delete() noexcept = default;

    template<typename U, typename = std::enable_if_t<std::is_convertible<U(*)[], T(*)[]>::value>>
    Default_delete(const Default_delete<U[]>& d) noexcept
    {
    }

    ~Default_delete() = default;

    void operator()(T* ptr) const
    {
        static_assert(sizeof(T) > 0,
                      "Can't delete a pointer to an incomplete type");
        delete []ptr;
    }

    template<typename U>
    void operator()(U* ptr) const = delete;
};

template<typename T, typename Deleter = cyy::Default_delete<T>>
class Unique_ptr
{
    static_assert(!std::is_rvalue_reference<Deleter>::value,
                  "Unique_ptr can't be initialized by a Deleter which is a rvalue reference");

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

    using pointer      = typename pointer_helper::type;
    using element_type = T;
    using deleter_type = Deleter;

    // constructor

    constexpr
    Unique_ptr() noexcept
        : data()
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");    }

    constexpr
    Unique_ptr(std::nullptr_t) noexcept
        : data()
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");    }

    explicit
    Unique_ptr(pointer p) noexcept
        : data(p)
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");
    }

    Unique_ptr(pointer p, std::conditional_t<std::is_reference<deleter_type>::value, 
                                             deleter_type, 
                                             const deleter_type&> d) noexcept
        : data(p, std::forward<decltype(d)>(d))
    {
    }

    Unique_ptr(pointer p, std::remove_reference_t<deleter_type>&& d)
        : data(p, std::forward<decltype(d)>(d))
    {
        static_assert(!std::is_reference<deleter_type>::value,
                      "rvalue bound to reference");
    }

    // TODO: check
    Unique_ptr(Unique_ptr&& u) noexcept
        : data(u.release(), std::forward<deleter_type>(u.get_deleter()))
    {
    }

    template<typename U, typename E, typename = std::enable_if_t<
             std::is_convertible<typename Unique_ptr<U, E>::pointer,
                                 pointer>::value &&
             !std::is_array<U>::value &&
             std::conditional_t<std::is_reference<deleter_type>::value,
                                std::is_same<E, deleter_type>,
                                std::is_convertible<E, deleter_type>>::value>>
    Unique_ptr(Unique_ptr<U, E>&& u) noexcept
        : data(u.release(), std::forward<E>(u.get_deleter()))
    {
    }

    // delete constructor from lvalue reference
    Unique_ptr(const Unique_ptr&) = delete;


    // destructor
    ~Unique_ptr()
    {
        if (data.get_ptr())
        {
            data.get_del()(data.get_ptr());
        }
    }

    // Note: cyy::Unique_ptr doesn't support construct from auto_ptr
    // actually cyy::auto_ptr doesn't exits


    // assignments

    Unique_ptr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    Unique_ptr& operator=(Unique_ptr&& r) noexcept
    {
        reset(r.release());
        get_deleter() = std::forward<deleter_type>(r.get_deleter());
        return *this;
    }

    template<typename U, typename E, typename = std::enable_if_t<
             !std::is_array<U>::value &&
             std::is_convertible<typename Unique_ptr<U, E>::pointer, pointer>::value &&
             std::is_assignable<deleter_type&, E&&>::value>>
    Unique_ptr& operator=(Unique_ptr<U, E>&& r) noexcept
    {
        reset(r.get_deleter());
        get_deleter() = std::forward<E>(r.get_deleter());
        return *this;
    }

    // delete assign from lvalue reference
    Unique_ptr& operator=(const Unique_ptr& r) = delete;

    // returns a pointer to the managed object and releases the ownership
    pointer release() noexcept
    {
        pointer p = data.get_ptr();
        data.get_ptr() = pointer();
        return p;
    }

    // replace the managed object
    void reset(pointer ptr = pointer()) noexcept
    {
        pointer old_ptr = data.get_ptr();
        data.get_ptr() = ptr;
        if (old_ptr)
            get_deleter()(old_ptr);
    }

    // swap the managed objects and associated deleters
    void swap(Unique_ptr& other) noexcept
    {
        data.swap(other.data);
    }

    // returns a pointer to the managed object
    pointer get() const noexcept
    {
        return data.get_ptr();
    }

    // returns the deleter that is used for destruction of the managed object
    Deleter& get_deleter() noexcept
    {
        return data.get_del();
    }

    // return the deleter that is used for destruction of the managed object
    const Deleter& get_deleter() const noexcept
    {
        return data.get_del();
    }

    // check if there is an associated managed object
    explicit operator bool() const noexcept
    {
        return get() == pointer() ? false : true;
    }

    // dereference pointer to the managed object
    std::add_lvalue_reference_t<T> operator*() const
    {
        return *data.get_ptr();
    }

    pointer operator->() const noexcept
    {
        return data.get_ptr();
    }

private:
    detail::Unique_ptr_data<pointer, deleter_type> data;

};

// specialization for manage array
template<typename T, typename Deleter>
class Unique_ptr<T[], Deleter>
{
    static_assert(!std::is_rvalue_reference<Deleter>::value,
                "Unique_ptr can't be initialized by a Deleter which is a rvalue reference");

    struct pointer_helper
    {
        template<typename U>
        static typename U::pointer test(typename U::pointer *);

        template<typename U>
        static T* test(...);

        using type = decltype(test<std::remove_reference_t<Deleter>>(nullptr));
    };

public:
    using pointer      = typename pointer_helper::type;
    using element_type = T;
    using deleter_type = Deleter;

    // constructors

    constexpr Unique_ptr() noexcept
        : data()
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");
    }

    constexpr Unique_ptr(std::nullptr_t) noexcept
        : data()
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");
    }

    explicit Unique_ptr(pointer p) noexcept
        : data(p)
    {
        static_assert(!std::is_pointer<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of pointer type");
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is of reference type");
    }

    Unique_ptr(pointer p, std::conditional_t<std::is_reference<deleter_type>::value,
                                             deleter_type,
                                             const deleter_type&> d) noexcept
        : data(p, std::forward<decltype(d)>(d))
    {
    }

    Unique_ptr(pointer p, std::remove_reference_t<deleter_type>&& d)
        : data(p, std::forward<decltype(d)>(d))
    {
        static_assert(!std::is_reference<deleter_type>::value,
                      "This constructor is ill-formed if Deleter is a reference type");
    }

    Unique_ptr(Unique_ptr&& u) noexcept
        : data(u.release(), u.get_deleter())
    {
    }

    Unique_ptr(const Unique_ptr&) = delete;

    // destructor
    ~Unique_ptr()
    {
        if (data.get_ptr() != nullptr)
        {
            data.get_del()(data.get_ptr());
        }
    }

    // assignment
    Unique_ptr& operator=(Unique_ptr&& r) noexcept
    {
        reset(r.release());
        data.get_del() = std::forward<deleter_type>(r.get_deleter());
        return *this;
    }

    Unique_ptr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    Unique_ptr& operator=(const Unique_ptr&) = delete;

    // returns a pointer to the managed object and releases the ownership
    pointer release() noexcept
    {
        pointer p = data.get_ptr();
        data.get_ptr() = pointer();
        return p;
    }

    // returns a pointer to the managed object and releases the ownership
    void reset(pointer ptr = pointer()) noexcept
    {
        pointer old_ptr = data.get_ptr();
        data.get_ptr() = ptr;
        if (old_ptr)
        {
            data.get_del()(old_ptr);
        }
    }

    // prevent using reset() with a pointer to derived.
    // (which would result in undefined behavior with arrays)
    template<typename U>
    void reset(U) = delete;

    void reset(std::nullptr_t) noexcept
    {
        reset();
    }

    // swaps the managed objects
    void swap(Unique_ptr& other) noexcept
    {
        data.swap(other.data);
    }

    // returns a pointer to the managed object
    pointer get() const noexcept
    {
        return data.get_ptr();
    }

    // returns the deleter that is used for destruction of the managed object
    deleter_type& get_deleter() noexcept
    {
        return data.get_del();
    }

    const deleter_type& get_deleter() const noexcept
    {
        return data.get_del();
    }

    // checks if there is an associated managed object
    explicit operator bool() const noexcept
    {
        return data.get_ptr() == pointer() ? false : true;
    }

    // provides indexed access to the managed array
    T& operator[](std::size_t i) const
    {
        return *(data.get_ptr() + i);
    }

private:
    detail::Unique_ptr_data<pointer, deleter_type> data;
};

// create a unique pointer that manages a new object
template<typename T, typename... Args>
Unique_ptr<T> make_unique(Args&&... args)
{
    return Unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespce cyy

#endif // UNIQUE_PTR
