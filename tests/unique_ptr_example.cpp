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

struct Vec3
{
    int x, y, z;
    Vec3() : x(0), y(0), z(0) { }
    Vec3(int x, int y, int z) :x(x), y(y), z(z) { }
    friend std::ostream& operator<<(std::ostream& os, Vec3& v) {
        return os << '{' << "x:" << v.x << " y:" << v.y << " z:" << v.z  << '}';
    }
};

int main()
{

    using cyy::Unique_ptr;

    std::cout << "\nTests for constructors:\n\n";
    {
        std::cout << "Example constructor(1)...\n";
        Unique_ptr<Foo> up1;  // up1 is empty
        static_assert(sizeof(up1) == sizeof(Foo*));
        Unique_ptr<Foo> up1b(nullptr);  // up1b is empty
        // Unique_ptr<Foo> upCopy(up1);
    
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
 
        std::cout << "Example array constructor...\n";
        {
            Unique_ptr<Foo[]> up(new Foo[3]);
        } // three Foo objects deleted
    }
    std::cout << "\nTests for relaese():\n\n";
    {
        struct Foo {
            Foo() { std::cout << "Foo\n"; }
            ~Foo() { std::cout << "~Foo\n"; }
        };

        std::cout << "Creating new Foo...\n";
        cyy::Unique_ptr<Foo> up(new Foo());
    
        std::cout << "About to release Foo...\n";
        Foo* fp = up.release();
    
        assert (up.get() == nullptr);
        std::cout << "Foo is no longer owned by unique_ptr...\n";
    
        delete fp;
    }

    std::cout << "\nTests for reset():\n\n";
    {
        struct Foo {
            Foo() { std::cout << "Foo...\n"; }
            ~Foo() { std::cout << "~Foo...\n"; }
        };
        
        struct D {
            void operator() (Foo* p) {
                std::cout << "Calling delete for Foo object... \n";
                delete p;
            }
        };

        std::cout << "Creating new Foo...\n";
        cyy::Unique_ptr<Foo, D> up(new Foo(), D());  // up owns the Foo pointer (deleter D)
    
        std::cout << "Replace owned Foo with a new Foo...\n";
        up.reset(new Foo());  // calls deleter for the old one
    
        std::cout << "Release and delete the owned Foo...\n";
        up.reset(nullptr);
    }

    std::cout << "\nTests for swap():\n\n";
    {
        struct Foo {
            Foo(int _val) : val(_val) { std::cout << "Foo...\n"; }
            ~Foo() { std::cout << "~Foo...\n"; }
            int val;
        };

        cyy::Unique_ptr<Foo> up1(new Foo(1));
        cyy::Unique_ptr<Foo> up2(new Foo(2));
    
        up1.swap(up2);
    
        std::cout << "up1->val:" << up1->val << std::endl;
        std::cout << "up2->val:" << up2->val << std::endl;
    }

    std::cout << "\nTests for get():\n\n";
    {
        cyy::Unique_ptr<std::string> s_p(new std::string("Hello, world!"));
        std::string *s = s_p.get();
        std::cout << *s << '\n';
    }

    std::cout << "\nTests for get_deleter():\n\n";
    {
        struct Foo
        {
            Foo() { std::cout << "Foo...\n"; }
            ~Foo() { std::cout << "~Foo...\n"; }
        };
        
        struct D
        {
            void bar() { std::cout << "Call deleter D::bar()...\n"; }
            void operator()(Foo* p) const
            {
                std::cout << "Call delete for Foo object...\n";
                delete p;
            }
        };

        cyy::Unique_ptr<Foo, D> up(new Foo(), D());
        D& del = up.get_deleter();
        del.bar();
    }

    std::cout << "\nTest for operator bool()\n\n";
    {
        cyy::Unique_ptr<int> ptr(new int(42));
    
        if (ptr) std::cout << "before reset, ptr is: " << *ptr << '\n';
        ptr.reset();
        if (ptr) std::cout << "after reset, ptr is: " << *ptr << '\n';
    }

    std::cout << "\nTest for operator* and operator->\n\n";
    {
        struct Foo {
            void bar() { std::cout << "Foo::bar\n"; }
        };
        
        auto f = [] (const Foo&)
        {
            std::cout << "f(const Foo&)\n";
        };

        cyy::Unique_ptr<Foo> ptr(new Foo);

        ptr->bar();
        f(*ptr);
    }

    std::cout << "\nTest for operator=\n\n";
    {
        struct Foo {
            Foo() { std::cout << "Foo\n"; }
            ~Foo() { std::cout << "~Foo\n"; }
        };
        
        cyy::Unique_ptr<Foo> p1;
        {
            std::cout << "Creating new Foo...\n";
            cyy::Unique_ptr<Foo> p2( cyy::make_unique<Foo>() );
            // p1 = p2; // Error ! can't copy unique_ptr
            p1 = std::move(p2);
            std::cout << "About to leave inner block...\n";
    
            // Foo instance will continue to live, 
            // despite p2 going out of scope
        }

        // cyy::Unique_ptr<Foo> p2;
        // p2 = p1;
    
        std::cout << "About to leave program...\n";
    }

    std::cout << "\n\nTests for unique_ptr for array\n\n";
    {
    const int size = 10; 
    cyy::Unique_ptr<int[]> fact(new int[size]);
 
    for (int i = 0; i < size; ++i) {
        fact[i] = (i == 0) ? 1 : i * fact[i-1];
    }
 
    for (int i = 0; i < size; ++i) {
        std::cout << i << ": " << fact[i] << '\n';
    }
    Unique_ptr<int[]> p(std::move(fact));
    for (int i = 0; i < size; ++i) {
        std::cout << p[i] << " ";
    }
    std::cout << "\n";
    p.reset();
    }

    std::cout << "\n\nTest for make_unique\n\n";
    {
        // Use the default constructor.
        cyy::Unique_ptr<Vec3> v1 = cyy::make_unique<Vec3>();
        // Use the constructor that matches these arguments
        cyy::Unique_ptr<Vec3> v2 = cyy::make_unique<Vec3>(0, 1, 2);
        // Create a unique_ptr to an array of 5 elements
        cyy::Unique_ptr<Vec3[]> v3 = cyy::make_unique<Vec3[]>(5);

        std::cout << "make_unique<Vec3>():      " << *v1 << '\n'
                  << "make_unique<Vec3>(0,1,2): " << *v2 << '\n'
                  << "make_unique<Vec3[]>(5):   " << '\n';
        for (int i = 0; i < 5; i++) {
            std::cout << "     " << v3[i] << '\n';
        }

        // compile error
        //cyy::make_unique<int[3]>(1,2,3,4);
    }
}