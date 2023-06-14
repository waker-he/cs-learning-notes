# Content

- [Part I: Basic Language Features](#part-i-basic-language-features)
    - [Chapter 1: Structured Bindings](#chapter-1-structured-bindings)
    - [Chapter 2: `if` and `switch` with Initialization](#chapter-2-if-and-switch-with-initialization)
    - [Chapter 3: Inline Variables](#chapter-3-inline-variables)


# Part I: Basic Language Features

## Chapter 1: Structured Bindings

### understand structured bindings
- Structured bindings allow you to decompose an object who has multiple members and use these members to initialize multiple entities respectively.
- __main benefit is code readability__: it allows you bind multiple values to seperate variables whose names that better describe their semantic meaning
- how it works: there is a hidden anonymous variable involved
    ```cpp
    struct A {
        int i = 0;
        std::string s;
    };
    A a;
    auto [x, y] = a;
    /*
    auto e = a;
    aliasname x = a.i;
    aliasname y = a.s;
    */
    // note: aliasname, not reference!
    ```

### usage of structured bindings
- classes with no non-static private member variables
    - if inheritance is involved, all public non-static member variables must be in the same class
- raw array (non-decay)
- classes with tuple-like API
    - `std::tuple`, `std::pair` (can also use `std::tie` to unpack)
    - `std::array`
    - user-defined
        - `std::tuple_size<type>::value`: number of elements
        - `std::tuple_element<i, type>::type`: type of ith element
        - `get<i>(tuple)`: yield value of ith element of object __tuple__

## Chapter 2: `if` and `switch` with Initialization

- `if` and `switch` control structures now allow us to specify an initialization clause before the usual condition clause.
- The initialized variable will be valid for the whole `if` statement
```cpp
if (std::lock_guard lg{mut}; false) {
    // i is not valid in this scope
}
else if (int i = 1; i) {
    // lg is still valid in this scope
}
else {
    // i and lg are both valid in this scope
}
```

## Chapter 3: Inline Variables

- review [static member variables rules](../README.md#rules-of-static-member-variables)
- since C++17, you can define an object/static member variable as `inline` in the header file to bypass __one definition rule__:
    - before C++17
        ```cpp
        class A {
            static const std::string msg;
        }
        const std::string A::msg = "ERROR";    // Link ERROR if included by multiple CPP files
        A a_obj;    // Link ERROR if included by multiple CPP files
        ```
    - since C++17:
        ```cpp
        class A {
            // OK to be included by multiple CPP files
            inline static const std::string msg = "OK";
        }
        // OK to be included by multiple CPP files
        inline A a_obj;
        ```
- __main benefit__: allows a single globally available object by defining it only in a header file
- __downside__: [magic statics](../README.md#magic-statics-since-c11)
