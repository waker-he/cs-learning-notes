# Content
- [Chapter1: Deducing Types](#chapter-1-deducing-types)
    - [item 1: C++98 template type deduction](#item-1-c98-template-type-deduction)
    - [item 2: auto type deduction (since C++11)](#item-2-auto-type-deduction-since-c11)


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