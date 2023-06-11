# Content
- [Chapter1: Deducing Types](#chapter-1-deducing-types)
    - [item 1: C++98 template type deduction](#item-1-c98-template-type-deduction)
    - [item 2: auto type deduction (since C++11)](#item-2-auto-type-deduction-since-c11)
    - [item 3: decltype (since C++11)](#item-3-decltype-since-c11)
    - [item 4: viewing deduced types](#item-4-viewing-deduced-types)
- [Chapter 2: auto](#chapter-2-auto)
    - [item 5: prefer auto to explicit type declaration](#item-5-prefer-auto-to-explicit-type-declarations)


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