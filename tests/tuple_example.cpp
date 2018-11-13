#include <vector>
#include <type_traits>
#include <iostream>
#include <string>
#include <memory>
#include <tuple>
#include <map>
#include <cassert>
#include <algorithm>
#include "tuple.h"

template<typename T, std::size_t I, typename = std::enable_if_t<cyy::is_tuple<T>::value>>
struct Tuple_printer
{
    static void print(const T& t)
    {
        Tuple_printer<T, I-1>::print(t);
        std::cout << ", " << cyy::get<I>(t);
    }
};

template<typename T>
struct Tuple_printer<T, 0>
{
    static void print(const T& t)
    {
        std::cout << cyy::get<0>(t);
    }
};

template<typename... Args>
void print(const cyy::Tuple<Args...>& t)
{
    std::cout << "(";
    Tuple_printer<decltype(t), sizeof...(Args)-1>::print(t);
    std::cout << ")\n";
}

int main()
{
    // std::tuple
    // tests for std::tuple constructor
    {
        cyy::Tuple<int, std::string, double> t1;
        std::cout << "Value-initialized: ";
        print(t1);
        cyy::Tuple<int, std::string, double> t2(42, "Test", -3.14);
        std::cout << "Initialized with values: ";
        print(t2);
        cyy::Tuple<char, std::string, int> t3(t2);
        std::cout << "Implicitly converted: ";
        print(t3);
        cyy::Tuple<int, double> t4(cyy::Tuple<int, double>(42, 3.14));
        std::cout << "Constructed from a pair";
        print(t4);
    }
    {
        std::vector<int> vi{1,2,3,4};
        auto t5 = cyy::make_tuple(std::ref(vi), 10, std::string("fuck"), 10.0f, 'v');
        std::cout << cyy::Tuple_size_v<decltype(t5)> << std::endl;
        std::cout << cyy::Tuple_size_v<decltype(cyy::make_tuple())> << std::endl;
        // cyy::Tuple_element_t<0, decltype(t5)> a = vi;
        static_assert(std::is_same_v<std::vector<int>&, cyy::Tuple_element_t<0, decltype(t5)>>, "fuck");
    }
    // given Allocator my_alloc with a single-argument constructor my_alloc(int)
    // use my_alloc(1) to allocate 10 ints in a vector
    // std::vector<int, std::alloc> v(10, 1, std::alloc(1));
    // use my_alloc(2) to allocate 10 ints in a vector in a tuple
    // cyy::Tuple<int, std::vector<int, my_alloc>, double> t5(std::allocator_arg,
    //                                                       my_alloc(2), 42, v,  -3.14);

    // tests for get
    {
        auto t = cyy::make_tuple(1, "Foo", 3.14);
        // 基于下标的访问
        std::cout << "(" << cyy::get<0>(t) << ", " << cyy::get<1>(t)
                  << ", " << cyy::get<2>(t) << ")\n";
        cyy::Tuple<int> tt{1};
        std::cout << std::is_same_v<int&, decltype(cyy::get<0>(t))> << std::endl;
        std::cout << std::is_same_v<int&, decltype(cyy::get<0>(tt))> << std::endl;
        std::cout << std::is_same_v<int&&, decltype(cyy::get<0>(std::move(tt)))> << std::endl;
    }
    {
        auto t = cyy::make_tuple(1, "Foo", 3.14);
        std::cout << "("  << cyy::get<int>(t) << ", " << cyy::get<const char (&)[4]>(t)
                  << ", " << cyy::get<double>(t) << ")\n";
        
        // compile error
        // auto t1 = cyy::make_tuple(1,2,3);
        // std::cout << cyy::get<int>(t1) << "\n";

        std::cout << std::is_same_v<int&&, decltype(cyy::get<int>(std::move(t)))> << "\n";

        std::cout << std::is_same_v<const int&&, decltype(cyy::get<int>(std::move(
            static_cast<const cyy::Tuple<int, const char(&)[4], double>&>(t))))> << "\n";
    }

    {
        struct Empty{};
        struct Large{char a[100];};
        // std::cout << 
        std::cout << sizeof(cyy::Tuple<Large, Empty, Empty, Large>) << "\n";
        std::cout << sizeof(std::tuple<Large, Empty, Empty, Large>) << "\n";
    }

    // test for cyy::forward_as_tuple
    {
        auto t1 = cyy::forward_as_tuple(10);
        auto t2 = cyy::forward_as_tuple(20, 'a');
    }

    // test for cyy::tie
    {
        auto func = [] () -> cyy::Tuple<int, double, int> {
            return cyy::make_tuple(1, 10.1, 2);
        };

        int i;
        double d;
        cyy::tie(i, d, cyy::ignore) = func();
        std::cout << i << " " << d << "\n";
    }

    // test for member function get
    {
        auto t = cyy::make_tuple(1,2,3.3, 5);
        t.get<0>() = 10;
        std::cout << cyy::get<0>(t) << "\n";
    }

    // tests for swap
    {
        auto f = [] (auto&& t1, auto&& t2) {
            print(t1);
            print(t2);
        };
        auto t1 = cyy::make_tuple(1,2,3);
        auto t2 = cyy::make_tuple(4,5,6);
        std::cout << "Before exchange: \n";
        f(t1, t2);
        cyy::swap(t1, t2);
        std::cout << "After exchange: \n";
        f(t1, t2);
    }

    // tests for compare between tuples
    {
        std::vector<cyy::Tuple<int, std::string, float>> v;
        v.emplace_back(2, "baz", -0.1);
        v.emplace_back(2, "bar", 3.14);
        v.emplace_back(1, "foo", 100.1);
        std::sort(v.begin(), v.end());
    
        for(auto p: v) {
            std::cout << "(" << cyy::get<0>(p) << ", " << cyy::get<1>(p)
                    << ", " << cyy::get<2>(p) << ")\n";
        }

        assert((v[0] <  v[1]) == true);
        assert((v[0] <= v[1]) == true);
        assert((v[0] >  v[1]) == false);
        assert((v[0] >= v[1]) == false);
        assert((v[0] == v[1]) == false);
        assert((v[0] != v[1]) == true);
    }
}
