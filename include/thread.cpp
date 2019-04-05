#include "thread.h"

using namespace cyy;
using namespace std;

void* cyy::detail::thread_entry(void* param)
{
    Unique_ptr<Thread_data_base> p(static_cast<Thread_data_base*>(param));
    p->run();
    return nullptr;
}

Thread::Thread(Thread&& other) noexcept
    : id_(), data_(nullptr)
{
    swap(std::move(other));
}

Thread::Thread::~Thread()
{
    if (joinable())
    {
        std::terminate();
    }
}

Thread& Thread::operator=(Thread&& other) noexcept
{
    if (joinable())
    {
        std::terminate();
    }
    swap(std::move(other));
    return *this;
}

bool Thread::joinable() const noexcept
{
    return id_ != id();
}

Thread::id Thread::get_id() const noexcept
{
    return id_;
}

Thread::native_handle_type Thread::native_handle()
{
    return id_.tid_;
}

void Thread::join()
{
    int err = EINVAL;
    if (id_ != id())
    {
        err = ::pthread_join(id_.tid_, nullptr);
    }
    if (err != 0)
    {
        throw std::system_error(std::error_code(err, std::generic_category()));
    }
    id_ = id();
}

void Thread::detach()
{
    int err = EINVAL;
    if (id_ != id()) {
        err = ::pthread_detach(id_.tid_);
    }
    if (err != 0)
    {
        throw std::system_error(std::error_code(err, std::generic_category()));
    }
    id_ = id();
}

void Thread::swap(Thread&& other) noexcept
{
    std::swap(id_.tid_, other.id_.tid_);
    std::swap(data_, other.data_);
}

