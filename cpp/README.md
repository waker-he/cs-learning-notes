- [Effective Modern C++](./effective-modern-c%2B%2B/effective-modern-cpp.md)
- [C++17](./c%2B%2B17/cpp17.md)
- [C++20](./c%2B%2B20/cpp20.md)

# General C++ notes

- [C++ Abstract Machine](./c++_abstract_machine/c++_abstract_machine.md)
- C++ Class
    - [Special Member Functions](./class/special_member_func.md)
    - [Class Layout](./class/class_layout.md)
    - [`dynamic_cast`](./class/dynamic_cast.md)
- C++ Objects
    - [Object Lifetime](./object_lifetime/object_lifetime.md)
    - [Initialization](./initialization/initialization.md)
    - [type-categories](./cppcon/type-categories.md)
- [template](./cppcon/template/template.md)
- [value-categories](./cppcon/value-categories/value-categories.md)
- [name lookup and overload resolution](./overload_resolution/overload_resolution.md)
- [allocator](./cppcon/allocator/allocator.md)
- [Application Binary Interface (ABI)](./cppcon/abi/abi.md)
- [CMake](./cmake/cmake.md)
- Concurrency and Parallelism
    - [C++ Concurrency in Action, by Anthony Willians](./concurrency/concurrency_in_action/concurrency_in_action.md)
    - [C++ Memory Model](./concurrency/memory-model/memory-model.md)
- C++ Standard Library
    - [Classic STL](./classic-stl/classic-stl.md)
    - [\<chrono>](./cppcon/chrono/chrono.md)
- others
    - [Helpful Online Resouces and Tools](#helpful-resouces-and-tools)
    - [C++ Runtime](#c-runtime)
    - [translation unit](#trasnlation-unit)
    - [linkage](#linkage)
    - [`using` keyword](#using-keyword)
    - [rules of static member variables](#rules-of-static-member-variables)
    - [magic statics (since C++11)](#magic-statics-since-c11)
    - [member pointers](#member-pointers)
    - [casting](#casting)
    - [reference vs pointer](#reference-vs-pointer)
    - [conversion operator](#conversion-operator)
    - [Trailing Return Types Advantages](#trailing-return-types-advantages)
    - [Hashing Aggregates](#hashing-aggregates)

# Helpful Resouces and Tools

- [Awesome Modern C++](https://github.com/rigtorp/awesome-modern-cpp)

# C++ Runtime

- The term "C++ runtime" refers to the runtime support provided to C++ programs, which includes both library support and certain behaviors implemented by the compiler that are required for the execution of a C++ program.
    - __library support__: runtime libraries (implicitly included) that provide support for dynamic memory management, exception handling, etc.
    - __support for language features__: exception handling, `dynamic_cast`, etc.
    - __startup and shutdown__: init and destroy variables of static storage, and call `main()` and return the return code of `main`


# trasnlation unit
- a basic unit of C++ compilation
- one translation unit generate one object file
- consists of one source file plus all the header files it includes

# linkage
- refers to the visibility of names across different translation units
- __external linkage__: can be seen by other translation units with a declaration to tell the compiler it exists
- __internal linkage__: can only be seen within the translation unit where it is declared
    - `static` global variable or functions
    - `const` global variable (can add `extern` to make it external)
    - any names in __anonymous namespaces__, including type declarations
- __no linkage__: can only be seen within the scope where it is declared
    - local variables in function (`static` or not)

# `using` keyword
- using-directives for namespaces and using-declarations for namespace members
    ```cpp
    using namespace std::literals;
    using std::cout;
    ```
- using-declarations for class members
    ```cpp
    struct B {
        virtual void f(int) {}
        void g(char) {}
        void h(int) {}
    protected:
        int m;
    }

    struct D : B {
        using B::m; // D::m is public
        using B::f, B::g, B::h; // since C++17: use comma
        void f(int) {}  // D::f(int) overrides B::f(int)

        void g(int) {}  // both g(int) and g(char) are visible

        void h(int) {}  // D::h(int) hides B::h(int)
    }
    ```
- type alias and alias template declaration (since C++11)
    ```cpp
    template <typename T>
    using vec = std::vector<T>;

    vec<int> v;
    ```

# rules of static member variables

## normal cases
- typically a static member variable is defined as follow:
    ```cpp
    // header Widget.h
    struct Widget {
        static int i;
    }
    ```

    ```cpp
    // Widget.cpp
    int Widget::i = 0;  // need out-of-line definition
    ```
- __linkage__: static member variable has external linkage (while static global variable has internal linkage)
- __one definition rule(ODR)__: the out-of-line definition can only appear once in the entire program (across multiple translation units)

## `const`, `constexpr`
- for `const static` member variable, if the type is __integral__ or __enumeration__ types, it can be initialized inside the class during declaration
- for `constexpr static` member variable, all literal types can be initialized inside the class during declaration
- for in-class initialization, it can be included in multiple CPP files and it does not violate ODR

## `inline` static variable (since C++17)

- the variable can be any type
```cpp
class A {
    // OK to be included by multiple CPP files
    inline static const std::string msg = "OK";
    inline static const std::string msg2;
};

// OK to be included by multiple CPP files
inline const std::string A::msg2{"OK2"};
```


# magic statics (since C++11)
- for `static` local variable in a function, [double-checked locking pattern](./concurrency/memory-model/memory-model.md#usage-in-double-checked-locking-pattern-dclp) is used to initialize it
- since C++17, with the introduction of [`inline` variable](./c%2B%2B17/cpp17.md#chapter-3-inline-variables), this also applies to `inline static` member variables
    - though it is always initialized before `main()` is called and I don't see any race condition that can happen with it
- to bypass: for literal type, use compile-time constant to initialize

# member pointers

Member pointers allow us to dynamically refer to a member of the class
```cpp
struct Point {
    int x;
    int y;
    static constexpr auto X = &Point::x;
    static constexpr auto Y = &Point::y;
    void decrease (int Point::* mem_ptr, int delta) {
        this->*mem_ptr -= delta;
    }
};

void increase(Point& point, int Point::* mem_ptr, int delta) {
    point.*mem_ptr += delta;
}

int main() {
    Point point{10, 20};
    increase(point, Point::X, 5);
    point.decrease(Point::Y, 6);
}
```

# casting

- `static_cast`: make implicit type conversion explicit, if not compatible, it will lead to a compile error.
- `dynamic_cast`
    - run-time cast, can be expensive
    - usually used to downcast a pointer or reference from a base type to a derived type
    - if fail:
        - return `nullptr`(when casting pointers)
        - throw `std::bad_cast`
- `const_cast`: only way to add or remove `const` qualifier in variable
- `reinterpret_cast`
    - mainly used in low-level pragramming
    - casting pointer/ref to any other pointer/ref

## advantage over C-style casts
- different notation or different casts, improve code maintainability
    - easily recognized and searchable
- eliminate unintended error
- perform all __valid__ operations that C casts can
    ```cpp
    int i = 1;
    char j = (char) i;  // compiled
    char k = reinterpret_cast<char>(i); // ERROR
    ```

# reference vs pointer

- from the assembly code perspective, reference is basically a `const` pointer with automatic dereference
    - `int&` equivalent to `int * const`
    - `const int&` equivalent to `const int * const`
- two exceptions:
    - cannot be null
    - `const reference` can bind to rvalue while `pointer to const` cannot
- despite its implementation, we should regard __reference__ as an _alias to an existing object_, it is the nature of __reference__ and the abstraction __reference__ provides


# conversion operator

- a special member function that a class can define to specify how to convert an object of its type to another type
    ```cpp
    class Foo {
    public:
    operator OtherType() const {}
    };
    ```
- can be defined as `explicit` to only allow conversion with `static_cast`
- example: `std::string` has a conversion operator to `std::string_view`, that is why `std::string` can be used where `std::string_view` is expected while `std::string_view` does not have a ctor takes `std::string`


# Trailing Return Types Advantages

> reference: [Embracing Trailing Return Types and `auto` Return SAFELY in Modern C++ - Pablo Halpern - CppCon 2022](https://www.youtube.com/watch?v=Tnl7FnwJ2Uw)
1. Omitting Namespace and Class Qualifiers

    ```cpp
    namespace NS {
        template <typename T> class C {};
        struct S {
            using E = int;
            auto f() -> E;
        };
    }

    // leading return type
    NS::C<NS::S::E> NS::S::f() {}
    // trailing return type
    auto NS::S::f() -> C<E> {}
    ```
2. Using a Runtime Argument by Name

    ```cpp
    template <typename A, typename B>
    decltype((std::declval<A>() + std::declval<B>()) / 2)
        avg(A a, B b)
    {
        return (a + b) / 2;
    }

    template <typename A, typename B>
    auto avg(A a, B b) -> decltype((a + b) / 2)
    {
        return (a + b) / 2;
    }
    ```
3. Readable Complex Return Types
    ```cpp
    // return pointer-to-function
    double (*f1(short s))(int *);
    auto f2(short s) -> double (*)(int *);

    // return pointer-to-class-member
    int (Calc::*g(kind k))(int);
    auto g(kind k) -> int (Calc::*)(int);
    ```
4. Lambda Expression can only use trailing return types
    - consistency between regular function and lambda expression syntax
5. Readability
    - function name is the most important thing of a function
    - trailing return type makes function names neatly aligned


# Hashing Aggregates

```cpp
struct AggregateHashser {
    template <class A>
    // requires tuple-like API
    size_t operator()(const A& a) const {
        auto hashComb = [](const auto&... t) {
            std::size_t seed = 0;
            return ((seed ^= std::hash<std::decay_t<decltype(t)>>{}(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);
        };

        return std::apply(hashComb, a);
    }
};
```