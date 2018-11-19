#include <iostream>
#include <memory>
#include "pointer_traits.h"

template <class Ptr>
struct BlockList
{
   // Predefine a memory block 
   struct block;
 
   // Define a pointer to a memory block from the kind of pointer Ptr s
   // If Ptr is any kind of T*, block_ptr_t is block*
   // If Ptr is smart_ptr<T>, block_ptr_t is smart_ptr<block>
   typedef typename cyy::pointer_traits<Ptr>::template rebind<block> block_ptr_t;
 
   struct block
   {
      std::size_t size;
      block_ptr_t next_block;
   }; 
 
   block_ptr_t free_blocks;
}; 

template<typename T>
struct P1
{
    using pointer         = T;
    using element_type    = int;
    using difference_type = int;
    static P1<T> pointer_to(int&)
    {
        return P1<T>{};
    }
};

template<>
struct P1<double>
{
};

int main()
{
    static_assert(std::is_same_v<cyy::pointer_traits<P1<int>>::pointer, P1<int>>, "fuck pointer");
    static_assert(std::is_same_v<cyy::pointer_traits<P1<int>>::element_type, int>, "fuck element type");
    static_assert(std::is_same_v<cyy::pointer_traits<P1<int>>::difference_type, int>, "fuck difference type");
    static_assert(std::is_same_v<cyy::pointer_traits<P1<double>>::pointer, P1<double>>, "fuck pointer");
    static_assert(std::is_same_v<cyy::pointer_traits<P1<double>>::element_type, double>, "fuck element type");
    static_assert(std::is_same_v<cyy::pointer_traits<P1<double>>::difference_type, std::ptrdiff_t>, "fuck difference type");
    static_assert(std::is_same_v<cyy::pointer_traits<std::shared_ptr<int>>::rebind<double>, std::shared_ptr<double>>, "fuck rebind");

    static_assert(std::is_same_v<cyy::pointer_traits<int*>::element_type, int>);
    static_assert(std::is_same_v<cyy::pointer_traits<int*>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<cyy::pointer_traits<int*>::rebind<double>, double*>);

    typedef std::pointer_traits<std::shared_ptr<int>>::rebind<double> another_pointer;
    static_assert(std::is_same<another_pointer, std::shared_ptr<double>>::value);

    int a;
    // complie error 
    // std::cout << cyy::pointer_traits<std::shared_ptr<int>>::pointer_to(a) << "\n";

    cyy::pointer_traits<P1<float>>::pointer_to(a);

    BlockList<int*> bl1;
    // The type of bl1.free_blocks is block*

    BlockList<std::shared_ptr<char>> bl2;
    // The type of bl2.free_blocks is std::shared_ptr<block>
    std::cout << bl2.free_blocks.use_count() << '\n';
}