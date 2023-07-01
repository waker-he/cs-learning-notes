- [Effective Modern C++](./effective-modern-c%2B%2B/effective-modern-cpp.md)
- [C++17](./c%2B%2B17/cpp17.md)
- [CppCon](./cppcon/README.md)

# General C++ notes

# Content
- [translation unit](#trasnlation-unit)
- [linkage](#linkage)
- [`using` keyword](#using-keyword)
- [rules of static member variables](#rules-of-static-member-variables)
- [magic statics (since C++11)](#magic-statics-since-c11)
- [member pointers](#member-pointers)
- [casting](#casting)
- [reference vs pointer]()

# trasnlation unit
- a basic unit of C++ compilation
- one translation unit generate one object file
- consists of one source file plus all the header files it includes

# linkage
- refers to the visibility of names across different translation units
- __external linkage__: can be seen by other translation units with a declaration to tell the compiler it exists
- __internal linkage__: can only be seen within the translation unit where it is declared
    - `static` global variable
    - `const` global variable (can add `extern` to make it external)
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
- __one definition rule(ODR)__: the out-of-line definition can only appear in one translation unit

## `const`, `constexpr`
- for `const static` member variable, if the type is __integral__ or __enumeration__ types, it can be initialized inside the class during declaration
- for `constexpr static` member variable, all literal types can be initialized inside the class during declaration
- for in-class initialization, it can be included in multiple CPP files and it does not violate ODR

## `inline` static variable (since C++17)
Refer to [here](./c%2B%2B17/cpp17.md#chapter-3-inline-variables)


# magic statics (since C++11)
- for `static` local variable in a function, there would be a thread-safe check to see if the `static` local variable has been initialized every time the function is called.
- since C++17, with the introduction of [`inline` variable](./c%2B%2B17/cpp17.md#chapter-3-inline-variables), this thread-safe check also applies to `inline static` member variables.
- to bypass:
    - for literal type, use compile-time constant to initialize
    - for `std::string`, use `std::string_view`

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
- perform all operations that C casts can

# reference vs pointer

- from the assembly code perspective, reference is basically a `const` pointer with automatic dereference
    - `int&` equivalent to `int * const`
    - `const int&` equivalent to `const int * const`
- two exceptions:
    - cannot be null
    - `const reference` can bind to rvalue while `pointer to const` cannot