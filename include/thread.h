#ifndef __THREAD_H
#define __THREAD_H

#include <ios>
#include <iostream>
#include <chrono>
#include <functional>
#include <utility>
#include <system_error>
#include <exception>
#include <pthread.h>
#include <time.h>

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

// store callable object and its parameters
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

// entry function used in pthread_create(), param is a pointer to Thread_data,
// Thread_data will run in the entry
void* thread_entry(void* param);

} // namespace detail

// wrapper class of pthread
class Thread
{
public:

    using native_handle_type = ::pthread_t;

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
        operator<<(std::basic_ostream<CharT, Traits>& ost, id p_id)
    {
        if (p_id == id())
            ost << "thread::id of a non-executing thread";
        else
        {
            ost << p_id.tid_;
        }
        return ost;
    }

    public:
        id(native_handle_type tid = 0) noexcept : tid_(tid) { }

    private:
        native_handle_type tid_;
    };

    // constructors
    Thread() noexcept
        : id_(), data_(nullptr)
    {
    }

    Thread(Thread&& other) noexcept;

    // create a callobject object and a thread
    template<typename Function, typename... Args>
    explicit Thread(Function&& f, Args&&... args)
        : data_(new detail::Thread_data<Function, Args...>(std::forward<Function>(f), std::forward<Args>(args)...))
    {
        if (int ret = ::pthread_create(&id_.tid_, 0, detail::thread_entry, (void*)data_); ret != 0)
        {
            throw std::system_error(std::error_code(ret, std::generic_category()));
        }
    }

    Thread(const Thread&) = delete;

    // destructor
    ~Thread();

    // moves the thread object
    Thread& operator=(Thread&& other) noexcept;

    // check whether the thread is joinable
    bool joinable() const noexcept;

    // return the id of the thread
    id get_id() const noexcept;

    // returns the underlying implementation-defined thread handle, pthread_t
    native_handle_type native_handle();

    // wait for a thread to finish its execution
    void join();

    // permit the thread to execute independently from the thread handle
    void detach();

    // swap two thread object
    void swap(Thread& other) noexcept;

    // return the number of concurrent threads
    static unsigned int hardware_concurrency() noexcept;

private:
    id id_;
    detail::Thread_data_base* data_;
};


namespace this_thread
{

extern thread_local Thread::id id;

inline Thread::id get_id() noexcept
{
    if (id == Thread::id())
    {
        id = Thread::id(::pthread_self());
    }
    return id;
}

inline void yield()
{
    ::pthread_yield();
}

template<typename Rep, typename Period>
void sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration)
{
    if (sleep_duration <= sleep_duration.zero())
        return;

    auto s  = std::chrono::duration_cast<std::chrono::seconds>(sleep_duration);
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_duration-s);

    ::timespec ts = { 
        static_cast<std::time_t>(s.count()),
        static_cast<long>(ns.count())
    };
    ::nanosleep(&ts, 0);
}

template<typename Clock, typename Duration>
void sleep_until(const std::chrono::time_point<Clock, Duration>& sleep_time)
{
    auto now = Clock::now();
    if (now < sleep_time)
        sleep_for(sleep_time - now);
}

} // namespace this_thread

} // namespace cyy

#endif // __THREAD_H
