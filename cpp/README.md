# General C++ notes

# Content
- [rules of static member variables](#rules-of-static-member-variables)

## rules of static member variables

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