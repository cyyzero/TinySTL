#include <iostream>
#include <memory>
#include <cassert>
#include "unique_ptr.h"
 
struct Foo { // object to manage
    Foo() { std::cout << "Foo ctor\n"; }
    Foo(const Foo&) { std::cout << "Foo copy ctor\n"; }
    Foo(Foo&&) { std::cout << "Foo move ctor\n"; }
    ~Foo() { std::cout << "~Foo dtor\n"; }
};
 
struct D { // deleter
    D() {};
    D(const D&) { std::cout << "D copy ctor\n"; }
    D(D&) { std::cout << "D non-const copy ctor\n";}
    D(D&&) { std::cout << "D move ctor \n"; }
    void operator()(Foo* p) const {
        std::cout << "D is deleting a Foo\n";
        delete p;
    };
};
 
int main()
{

    using cyy::Unique_ptr;

    std::cout << "\nTests for constructors\n\n";
    {
    std::cout << "Example constructor(1)...\n";
    Unique_ptr<Foo> up1;  // up1 is empty
    static_assert(sizeof(up1) == sizeof(Foo*));
    Unique_ptr<Foo> up1b(nullptr);  // up1b is empty
 
    std::cout << "Example constructor(2)...\n";
    {
        Unique_ptr<Foo> up2(new Foo); //up2 now owns a Foo
    } // Foo deleted
 
    std::cout << "Example constructor(3)...\n";
    D d;
    {  // deleter type is not a reference
       Unique_ptr<Foo, D> up3(new Foo, d); // deleter copied
    }
    {  // deleter type is a reference 
       Unique_ptr<Foo, D&> up3b(new Foo, d); // up3b holds a reference to d
    }
 
    std::cout << "Example constructor(4)...\n";
    {  // deleter is not a reference 
       Unique_ptr<Foo, D> up4(new Foo, D()); // deleter moved
       static_assert(sizeof(up4) == sizeof(Foo*));
    }
 
    std::cout << "Example constructor(5)...\n";
    {
       Unique_ptr<Foo> up5a(new Foo);
       Unique_ptr<Foo> up5b(std::move(up5a)); // ownership transfer
    }
 
    std::cout << "Example constructor(6)...\n";
    {
        Unique_ptr<Foo, D> up6a(new Foo, d); // D is copied
        Unique_ptr<Foo, D> up6b(std::move(up6a)); // D is moved
 
        Unique_ptr<Foo, D&> up6c(new Foo, d); // D is a reference
        // on x86-64 architecture
        static_assert(sizeof(up6c) == 16);
        Unique_ptr<Foo, D> up6d(std::move(up6c)); // D is copied
    }
 
    // std::cout << "Example array constructor...\n";
    // {
    //     Unique_ptr<Foo[]> up(new Foo[3]);
    // } // three Foo objects deleted
    }
}