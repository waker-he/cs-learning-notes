# General C++ notes

# Content
- [translation unit](#trasnlation-unit)
- [rules of static member variables](#rules-of-static-member-variables)
- [magic statics (since C++11)](#magic-statics-since-c11)
- [member pointers](#member-pointers)

# trasnlation unit
- a basic unit of C++ compilation
- one translation unit generate one object file
- consists of one source file plus all the header files it includes

# rules of static member variables

### normal cases
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

### `const`, `constexpr`
- for `const static` member variable, if the type is __integral__ or __enumeration__ types, it can be initialized inside the class during declaration
- for `constexpr static` member variable, all literal types can be initialized inside the class during declaration
- for in-class initialization, it can be included in multiple CPP files and it does not violate ODR

### `inline` static variable (since C++17)
Refer to [here](./c%2B%2B17/README.md#chapter-3-inline-variables)


# magic statics (since C++11)
- for `static` local variable in a function, there would be a thread-safe check to see if the `static` local variable has been initialized every time the function is called.
- since C++17, with the introduction of [`inline` variable](./c%2B%2B17/README.md#chapter-3-inline-variables), this thread-safe check also applies to `inline static` member variables.
- to bypass:
    - for literal type, use compile-time constant to initialize
    - for `std::string`, use `std::string_view`

# member pointers

Member pointers allow us to dynamically refer to a member of the class
```cpp
struct Point {
    int x;
    int y;
    static constexpr auto X = &Pointer::x;
    static constexpr auto Y = &Pointer::y;
    void decrease (int Point::mem_ptr, int delta) {
        this->*mem_ptr -= delta;
    }
};

void increase(Point& point, int Point::*mem_ptr, int delta) {
    point.*mem_ptr += delta;
}

int main() {
    Point point{10, 20};
    increase(point, Point::X, 5);
    point.decrease(Point::Y, 6);
}
```