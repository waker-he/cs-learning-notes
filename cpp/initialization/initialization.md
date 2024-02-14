# Initialization in C++

- [initialization rules from C](#intialization-rules-from-c)
    - [aggregate initialization](#aggregate-initialization)
    - [copy-initialization](#copy-initialization)
    - [default-initialization](#default-initialization)
- [From C to C++](#from-c-to-c)
    - [direct-initialization](#direct-initialization)
    - [value-initialization](#value)
    - [list-initialization](#list-initialization-since-c11)

## intialization rules from C

- __C doesn't name the forms of initialization like C++ does__

### aggregate initialization

initialization of [aggregate type](../cppcon/type-categories.md#aggregates)

```cpp
T object = {arg1, arg2, ...};               // since C
T object{arg1, arg2, ...};                  // since C++11
T object = { .des1=arg1, .des2{arg2}, ...}; // since C++20
T object { .des1=arg1, .des2{arg2}, ...};   // since C++20
```

### copy-initialization

__Initializes an object from another object. (An initializer is given via `=`)__
```cpp
T object = other;       // T is not reference
// - { pass | return | throw | catch }-by-value
// - member init during aggregate initialization
```

### default-initialization

__This is the initialization performed when an object is constructed with no initializer.__
```cpp
T object;
```

- __scalar type__ or __agggregate__:
    - if static or thread storage duration, it's __zero-initialized__
    - else, it is left uninitialized
- __class type__: attempt to call its default ctor

## From C to C++

- most of C++98/03's new initialization rules have to do with __object-oriented programming__ (class types, ctors and object lifetime)
    - __Object-Oriented Programming__
        - __class types__
        - --> aggregate initialization cannot help maintain __class invariant__, it becomes the user's responsibility to enforce __class invariant__ themselves
        - --> need of __ctors__
        - --> [direct-initialization](#direct-initialization) and object lifetime redefinition
    - C doesn't distinguish between storage duration and object lifetime, but in C++, an object's lifetime begins after it's been initialized
        - so in `int x;`, `x` is considered default initialized or it's called vacuous initialization

### direct-initialization

__providing ctor arguments in parentheses. It is versus copy-init.__

```cpp
T object(args...);      // calling copy ctor here is also direct-init
new T(args...);
static_cast<T>(other);  // expression results in a prvalue of T
```
- C++ strives to treat user-defined types and built-in types uniformly:
    - scalar types: `int x(5);`
    - agggregates   (since C++20)
- to use `explicit` ctor or `explicit` conversion operator to initialize an object, must use direct-init instead of copy-init
    ```cpp
    struct A {
        explicit A(int i) {}
        explicit operator double() {}
    };
    A a(2);             // OK
    A a = 2;            // ERROR
    double d(A(1));     // OK
    double d = A(1);    // ERROR
    ```

### value-initialization

__This is the initialization performed when an object is constructed with an empty initializer. It is versus default-init.__

```cpp
T(), new T();
int x = int();  // x == 0, comb of value-init and copy-init
T object{};     // since C++11
```

- if `T` is scalar type or class type with a trivial default ctor, it is __zero-initialized__
    - this is the case when it differs from __default-initialization__
- otherwise, attemp to call its default ctor

### list-initialization (since C++11)

__Initializes an object from braced-init-list.__

```cpp
T object{args...};      // direct-list-initialization
T object = {args...};   // copy-list-initialization, T(Args...) should be non-explicit
```

- check [Effective Modern C++: item 7: uniform/brace initialization (since C++11)](../effective-modern-c++/effective-modern-cpp.md#item-7-uniformbrace-initialization-since-c11)

## Reference

- [Back to Basics: Initialization in C++ - Ben Saks - CppCon 2023](https://www.youtube.com/watch?v=_23qmZtDBxg)
- cppreference
    - [Aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
    - [Copy-initialization](https://en.cppreference.com/w/cpp/language/copy_initialization)
    - [Default-initialization](https://en.cppreference.com/w/cpp/language/default_initialization)
    - [Default constructors](https://en.cppreference.com/w/cpp/language/default_constructor)
    - [Direct-initialization](https://en.cppreference.com/w/cpp/language/direct_initialization)
    - [Value-initialization](https://en.cppreference.com/w/cpp/language/value_initialization)
    - [List-initialization](https://en.cppreference.com/w/cpp/language/list_initialization)