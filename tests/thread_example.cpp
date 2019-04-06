#include "thread.h"

#include <iostream>
#include <mutex>
#include <utility>
#include <chrono>
#include <cassert>
#include <unistd.h>

void f1(int n)
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread 1 executing\n";
        ++n;
        cyy::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
 
void f2(int& n)
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "Thread 2 executing\n";
        ++n;
        cyy::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
 
class foo
{
public:
    void bar()
    {
        for (int i = 0; i < 5; ++i) {
            std::cout << "Thread 3 executing\n";
            ++n;
            cyy::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    int n = 0;
};
 
int main()
{

    std::cout << "\nTest for this_thread::get_id():\n";
    {
        std::mutex g_display_mutex;
        auto foo = [&] () {
            auto this_id = cyy::this_thread::get_id();
        
            g_display_mutex.lock();
            std::cout << "thread " << this_id << " sleeping...\n";
            g_display_mutex.unlock();
        
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
        };
        cyy::Thread t1(foo);
        cyy::Thread t2(foo);
        t1.join();
        t2.join();

    }

    std::cout << "\nTest for this_thread::yield():\n";
    {
        auto little_sleep = [] (std::chrono::microseconds us) {
            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + us;
            do {
                cyy::this_thread::yield();
            } while (std::chrono::high_resolution_clock::now() < end);
        };
        auto start = std::chrono::high_resolution_clock::now();
        little_sleep(std::chrono::microseconds(100));
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        std::cout << "waited for "
                  << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
                  << " microseconds\n";
    }

    std::cout << "\nTest for this_thread::sleep_for():\n";
    {
        using namespace std::chrono_literals;
        std::cout << "Hello waiter\n" << std::flush;
        auto start = std::chrono::high_resolution_clock::now();
        cyy::this_thread::sleep_for(2s);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        std::cout << "Waited " << elapsed.count() << " ms\n";
    }

    std::cout << "\nTest for constructor:\n";
    {
        int n = 0;
        foo f;
        cyy::Thread t1; // t1 is not a thread
        cyy::Thread t2(f1, n + 1); // pass by value
        cyy::Thread t3(f2, std::ref(n)); // pass by reference
        cyy::Thread t4(std::move(t3)); // t4 is now running f2(). t3 is no longer a thread
        cyy::Thread t5(&foo::bar, &f); // t5 runs foo::bar() on object f
        t2.join();
        t4.join();
        t5.join();
        assert(n == 5);
        std::cout << "Final value of n is " << n << '\n';
        assert(f.n == 5);
        std::cout << "Final value of foo::n is " << f.n << '\n';
    }

    std::cout << "\nTest for joinable():\n";
    {
        auto foo = [] () { };
        cyy::Thread t;
        assert(!t.joinable());
        std::cout << "before starting, joinable: " << std::boolalpha << t.joinable()
                  << '\n';
    
        t = cyy::Thread(foo);
        assert(t.joinable());
        std::cout << "after starting, joinable: " << t.joinable() 
                  << '\n';
    
        t.join();
        assert(!t.joinable());
        std::cout << "after joining, joinable: " << t.joinable() 
                  << '\n';
    }

    std::cout << "\nTest for join():\n";
    {
        auto foo = [] () {
            // simulate expensive operation
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
        };
        auto bar = [] () {
            // simulate expensive operation
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
        };
        std::cout << "starting first helper...\n";
        cyy::Thread helper1(foo);
    
        std::cout << "starting second helper...\n";
        cyy::Thread helper2(bar);
    
        std::cout << "waiting for helpers to finish..." << std::endl;
        helper1.join();
        helper2.join();
        std::cout << "done!\n";
    }

    std::cout << "\nTest for detach():\n";
    {
        auto independentThread = [] () {
            std::cout << "Starting concurrent thread.\n";
            cyy::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "Exiting concurrent thread.\n";
        };
        auto threadCaller = [&] () {
            std::cout << "Starting thread caller.\n";
            cyy::Thread t(independentThread);
            t.detach();
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Exiting thread caller.\n";
        };
        threadCaller();
        cyy::this_thread::sleep_for(std::chrono::seconds(5));
    }

    std::cout << "\nTest for swap():\n";
    {
        auto foo = [] () {
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
        };
        auto bar = [] () {
            cyy::this_thread::sleep_for(std::chrono::seconds(1));
        };
        cyy::Thread t1(foo);
        cyy::Thread t2(bar);

        auto id1 = t1.get_id();
        auto id2 = t2.get_id();

        std::cout << "thread 1 id: " << t1.get_id() << std::endl;
        std::cout << "thread 2 id: " << t2.get_id() << std::endl;
    
        std::swap(t1, t2);
        assert(id1 == t2.get_id() && id2 == t1.get_id());
        std::cout << "after std::swap(t1, t2):" << std::endl;
        std::cout << "thread 1 id: " << t1.get_id() << std::endl;
        std::cout << "thread 2 id: " << t2.get_id() << std::endl;
    
        t1.swap(t2);
        assert(id1 == t1.get_id() && id2 == t2.get_id());
        std::cout << "after t1.swap(t2):" << std::endl;
        std::cout << "thread 1 id: " << t1.get_id() << std::endl;
        std::cout << "thread 2 id: " << t2.get_id() << std::endl;
    
        t1.join();
        t2.join();
    }

    std::cout << "\nTest for hardware_concurrency():\n";
    {
        unsigned int n = cyy::Thread::hardware_concurrency();
        std::cout << n << " concurrent threads are supported.\n";
    }



    // std::cout <<

}