#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>

#include "tuple.h"

namespace cyy
{

namespace detail
{

struct Thread_data_base
{
    Thread_data_base() = default;
    virtual ~Thread_data_base();
    virtual void run();
};

template<typename Function, typename... Args>
struct Thread_data
{
    template<typename Function, typename... Args>
    Thread_data(Function&& f, Args... args)
        : data(std::forward<Function>(f), std::forward<Args>(args)...)
    {
    }

    template<std::size_t... Indices>
    void run_impl()
    {
    }

    void run() override
    {

    }

    Tuple<Function, Args...> data;

};

} // namespace detail

class Thread
{
public:
    Thread() noexcept = default;
    Thread(Thread&& other) noexcept;

    template<typename Function, typename... Args>
    explicit Thread(Function&& f, Args&&... args)
    {
        data = new detail::Thread_data_base(std::forward<Function>(f), std::forward<Args>(args)...);
        
    }

    Thread(const thread&) = delete;

private:
    detail::Thread_data_base* data;
};

} // namespace cyy

#endif // __THREAD_H