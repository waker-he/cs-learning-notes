## [Index](../README.md)

# Reflection

- [Definition](#definition)
- [Current C++](#current-c)
- [Past Attempt: Reflection TS](#past-attempt-reflection-ts)
- [P2996: Reflection in C++26](#p2996-reflection-for-c26)
    - [`std::meta::info`](#stdmetainfo)
    - [reflection operator (^)](#reflection-operator-)
    - [splicer `[: :]`](#splicer--)
    - [Metafunctions](#metafunctions)

## Definition

- introspection
    - Program observes the structure of itself
        - if not itself, then it is static analysis tool, not reflection
    - Program asks questions about itself
        - need unkonwn thing (usually type) to ask questions about
        - program doesn't know what type it is, but compiler does
    - Analogy:
        - ✅ X-Ray: observe the internal structure
        - ❌ Mirror: only observe the external surface
    - __Reflection queries__ work on __"reflections"__ (or reflection values)
        - __reflection operator__ transitions code to reflection
            - X-ray machine: real world object (right leg) -> reflection value (view of bones inside leg)
- code injection (also known as reflective metaprogramming)
    - results of reflection queries are used to generate the code
    - opposite of introspection: reflection -> code

## Current C++

- `std::type_info`
    - extremely limited runtime reflection, used to implement `dynamic_cast` and exceptions
    - `.name()` is the only useful information for programmers
- type traits
    - compile-time introspection of types
    - but only of types:
        - can't ask about class members or parent scope
        - can't ask about namespaces or functions or templates
    - very limited manipulation of types: add/remove const, volatile, pointer, ref
- `requires` expression
    - compile-time introspection of code validity
    - more like mirror, not introspection like x-ray

## Past Attempt: Reflection TS

- compile-time template metaprogramming
- a reflection is a unique type
- type metafunctions to query/manipulate reflections
```cpp
using r_str = reflexpr(std::string);       
using string = get_reflected_type_t<r_str>;
```
- not accepted since it is based on template metaprogramming

## P2996: Reflection for C++26

- changed paradigm from template metaprogramming to `consteval` function
- reflections are `constexpr` values of an opaque scalar type
- queries/manipulations done with `consteval` functions
- reflection to code done with special syntax "splicers"
- minimal viable proposal
    - more introspection
    - less code injection

### `std::meta::info`

- opaque scalar type
- holds reflection information for something
- only useful at compile-time
- only type for reflection value
    - why one type for everything?
        - language will change over time, easier to react to changes
        - `std::vector<std::meta::info>` can be represented

### reflection operator (^)

- converts grammatical construct into reflection value
```cpp
constexpr std::meta::info x = ^std::vector; // reflection of template std::vector
constexpr std::meta::info x = ^::;          // reflection of global namespace
constexpr std::meta::info x = ^(std::barrier<>::max() - 100);
```

### splicer `[: :]`

- converts reflection value into code
- operand must be a `std::meta::info`
- sometimes preceded by `typename` or `template`

```cpp
constexpr auto r = ^int;
typename[:r:] x= 42;
auto c = static_cast<[:^int:]>('*');

struct A { int z; };
constexpr auto member = ^A::z;
A a;
a.[:member:] = 42;
```

### Metafunctions

- inputs are `std::meta::info`
- outputs are information about the reflection value
- in namespace `std::meta`
- `consteval` functions
- examples:
    - query what kind of thing the reflection represents
        ```cpp
        consteval bool is_namespace(info r);
        consteval bool is_function(info r);
        ```
    - query names
        ```cpp
        consteval string_view name_of(info r);
        consteval string_view qualified_name_of(info r);
        ```
    - type properties: like `<type_traits>`
    - properties
        - `is_public`, `has_internal_linkage`, `is_virtual`, ...
    - members
        ```cpp
        template <class Predicate>
        consteval vector<info> members_of(info r, Predicate p);
        consteval vector<info> bases_of(info r);
        ```
    - tempalte
        ```cpp
        consteval bool has_template_arguments(info r);
        consteval info template_of(info r);
        consteval vector<info> template_arguments_of(info r);
        ```


## Reference

- [C++ Reflection - Back on Track - David Olsen - C++Now 2024](https://www.youtube.com/watch?v=nBUgjFPkoto)
