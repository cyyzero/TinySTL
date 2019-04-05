#ifndef __THREAD_H
#define __THREAD_H

#include <thread>
#include <functional>
#include <utility>
#include <system_error>
#include <exception>
#include <pthread.h>

#include "unique_ptr.h"
#include "integer_sequence.h"
#include "tuple.h"

namespace cyy
{
namespace detail
{

struct Thread_data_base
{
    Thread_data_base() = default;
    virtual ~Thread_data_base() = default;
    virtual void run() = 0;
};

template<typename Function, typename... Args>
struct Thread_data : public Thread_data_base
{
    Thread_data(Function&& f, Args&&... args)
        : func(std::forward<Function>(f)),
          param(cyy::make_tuple(std::forward<Args>(args)...))
    {
    }

    template<std::size_t... Idx>
    void run_impl(Index_sequence<Idx...>)
    {
        std::invoke(func, cyy::get<Idx>(param)...);
    }

    void run() override
    {
        using Idx_seq = Make_index_sequence<Tuple_size<decltype(param)>::value>;
        run_impl(Idx_seq{});
    }

    std::decay_t<Function> func;
    Tuple<typename decay_and_strip<Args>::type...> param;
};

void* thread_entry(void* param);

} // namespace detail

namespace this_thread
{
    // __thread 

} // namespace this_thread

class Thread
{
public:
    class id
    {
    friend class Thread;

    friend bool operator==(id lhs, id rhs)
    {
        return lhs.tid_ == rhs.tid_;
    }

    friend bool operator!=(id lhs, id rhs)
    {
        return lhs.tid_ != rhs.tid_;
    }

    friend bool operator<=(id lhs, id rhs)
    {
        return lhs.tid_ <= rhs.tid_;
    }

    friend bool operator< (id lhs, id rhs)
    {
        return lhs.tid_ < rhs.tid_;
    }

    friend bool operator> (id lhs, id rhs)
    {
        return lhs.tid_ > rhs.tid_;
    }

    friend bool operator>=(id lhs, id rhs)
    {
        return lhs.tid_ >= rhs.tid_;
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& ost, id id)
    {
        ost << id.tid_;
        return ost;
    }

    public:
        id() noexcept : tid_(0) { }
    private:
        ::pthread_t tid_;
    };

    using native_handle_type = ::pthread_t;

    Thread() noexcept
        : id_(), data_(nullptr)
    {
    }

    Thread(Thread&& other) noexcept;

    template<typename Function, typename... Args>
    explicit Thread(Function&& f, Args&&... args)
        : data_(new detail::Thread_data<Function, Args...>(std::forward<Function>(f), std::forward<Args>(args)...))
    {
        if (int ret = ::pthread_create(&id_.tid_, NULL, detail::thread_entry, (void*)data_); ret != 0)
        {
            throw std::system_error(std::error_code(ret, std::generic_category()));
        }
    }

    Thread(const Thread&) = delete;

    ~Thread();

    Thread& operator=(Thread&& other) noexcept;

    bool joinable() const noexcept;

    id get_id() const noexcept;

    native_handle_type native_handle();

    void join();

    void detach();

    void swap(Thread&& other) noexcept;

private:
    id id_;
    detail::Thread_data_base* data_;
};

} // namespace cyy

#endif // __THREAD_H
