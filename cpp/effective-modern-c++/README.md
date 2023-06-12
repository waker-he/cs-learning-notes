# Content
- [Chapter1: Deducing Types](#chapter-1-deducing-types)
    - [item 1: C++98 template type deduction](#item-1-c98-template-type-deduction)
    - [item 2: auto type deduction (since C++11)](#item-2-auto-type-deduction-since-c11)
    - [item 3: decltype (since C++11)](#item-3-decltype-since-c11)
    - [item 4: viewing deduced types](#item-4-viewing-deduced-types)
- [Chapter 2: auto](#chapter-2-auto)
    - [item 5: prefer auto to explicit type declaration](#item-5-prefer-auto-to-explicit-type-declarations)
    - [item 6: use the explicitly typed initializer idiom when auto deduces undesired types.](#item-6-use-the-explicitly-typed-initializer-idiom-when-auto-deduces-undesired-types)
- [Chapter 3: Moving to Modern C++](#chapter-3-moving-to-modern-c)
    - [item 7: uniform/brace initialization (since C++11)](#item-7-uniformbrace-initialization-since-c11)
    - [item 8: prefer nullptr to 0 or NULL](#item-8-prefer-nullptr-to-0-and-null)
    - [item 9: prefer alias declarations to typedefs](#item-9-prefer-alias-declarations-to-typedefs)
    - [item 10: prefer scoped enums to unscoped enums](#item-10-prefer-scoped-enums-to-unscoped-enums)
    - [item 11: prefer deleted functions to private undefined one](#item-11-prefer-deleted-functions-to-private-undefined-one)


# Chapter 1: Deducing Types

## item 1: C++98 template type deduction

Deduction of type `T` depends on `ParamType` and `ArgType`.
```cpp
template <typename T>
void f(ParamType param);

f(arg); // arg is of type ArgType
```

Rules:
```
if ArgType is array or function:
    ArgType does not decay to pointer if ParamType is reference

if ParamType is universal reference and arg is lvalue:
    T, ParamType = lvalue reference
    return
else if pass by value:
    Ignore qualifiers of ArgType
Ignore reference-ness of ArgType
Pattern-match the rest of ArgType against ParamType to determine T
```


## item 2: auto type deduction (since C++11)

The same as C++98 template type duduction:
```
auto == T
ParamType == type specifier (eg. const auto&)
```

One exception:
- template type deduction (and function auto return type deduction since C++14) cannot deduce type of braced list.
- auto type deduction deduced braced list as `std::initializer_list`
    - fixed in C++17: direct list initialization (w/o assignment operation `=`)
    ```cpp
    auto a{42};     // initialize as int
    auto b{42, 1};  // ERROR
    ```

## item 3: decltype (since C++11)
`decltype` echoes back whatever type of the expression passed in.
- for names, return the declaration type of that name
- for lvalue expression other than names, return reference type
```cpp
int x = 0;
// decltype(x) is int
// decltype((x)) is int&
```

Useful for `trailing return type` until C++11. (C++14 supports return
type deduction for all functions and lambdas.)

C++14 supports `decltype(auto)` to use `decltype` rule to deduce auto 
return/variable type.


## item 4: viewing deduced types
- During editing: IDE Editors
  - hover the curosr
  - may be unreliable
- During compiling: Compiler diagnostics
  - deliberately elicit an error message:
    ```cpp
    template <typename T> // declaration only for TD;
    class TD;             // TD == "Type Disaplayer"

    TD<decltype(x)> xType;
    ```
- During runtime: <boost/type_index.hpp>
    ```cpp
    std::cout << boost::typeindex::type_id_with_cvr<T>().pretty_name();
    ```


# Chapter 2: auto

## item 5: prefer auto to explicit type declarations
- it forces initialization when declaring variables
- less typing
- readability issue not as severe as expected, which is demonstrated by
dynamically typed languages such as Python
- more portable (32-bit and 64-bit size_t)
- more efficient
    - for closures, using `std::function` instead of `auto` to store a closure object might involve allocating heap memory to store the closure.
    - in case of mismatches
        ```cpp
        for (const pair<int, int>& p : map<int, int>) {
            // create a copy of pair in each iteration
            // should be const pair<const int, int>&
        }
        ```

## item 6: use the explicitly typed initializer idiom when auto deduces undesired types

- "invisible" proxy classes can cause `auto` to deduce the "wrong" type for an initializing expression.
- Use explicitly typed initializer idiom in this case:
    ```cpp
    auto x = T{y};
    ```
- Why not just `T x = y`, refering to [stackoverflow](https://stackoverflow.com/questions/25607216/why-should-i-prefer-the-explicitly-typed-initializer-idiom-over-explicitly-giv) (still not fully convincing):
    - The modern C++ style prefers the type on the right side
    - `{}` guarantees no narrowing.

# Chapter 3: Moving to Modern C++

## item 7: uniform/brace initialization (since C++11)
- pros:
  - versatile for initializing objects
  - no narrowings
  - immune to C++'s most vexing parse
    ```cpp
    Widget w1();    // function declaration
    Widget w1{};    // call Widget ctor with no args
    ```
- cons:
  - ctor with parameter type `std::initializer_list` would always be matched first if there is possible implicit type conversion from __the types of expressions within the braced list__ to __the element type of the `std::intializer_list`__ even though braced list does not allow narrowing.
    - exception:
      ```cpp
      Widget w{};   // call Widget ctor with no args
      Widget w({}); // call Widget ctor with empty std::initializer_list
      ```

## item 8: prefer nullptr to 0 and NULL
- `nullptr` is of type `std::nullptr_t` and can only be converted to pointer type (and it can only be implicitly converted to boolearn when a test expression is expected)
    ```cpp
    bool b = nullptr;   // ERROR
    if (nullptr) {}     // OK
    ```
- using 0 or NULL can cause trouble in overload resolution:
  - 0 can be implicitly converted to pointer type
  - NULL depends on implementation, might be 0, 0L or nullptr

## item 9: prefer alias declarations to typedefs
- alias declarations (using `using` keyword) support templatization
    ```cpp
    template <typename T>
    using vec = std::vector<T>;
    // vec<int> == std::vector<int>
    ```
- avoids `typename` prefix which is needed for compiler to distinguish between static variable and typename when the type is dependent on a template type parameter. (used in `type_traits`)
    ```cpp
    template <typename T>
    struct vec {
        typedef std::vector<T> type
    }

    template <typename T>
    struct Widget {
        typename vec<T>::type vec1;
    }
    ```

## item 10: prefer scoped enums to unscoped enums
- scoped enum does not leak names
- scoped enum does not allow implicit type conversion
    ```cpp
    enum class Color { white };
    Color i = white;        // ERROR
    int j = Color::white;   // ERROR
    ```
- scoped enum allows forward declaration without declaring underlying type, since the underlying type is defined to be `int` by default
    ```cpp
    enum Color;             // ERROR
    enum Color: int;        // OK
    enum class Color;       // OK, underlying type: int
    ```

## item 11: prefer deleted functions to private undefined one
- allows non member function to be deleted, can be used in:
  - disable implicit type conversions for arguments of function
  - disable specific template instantiations
- Better error message (`deleted` instead of `unable to access`)