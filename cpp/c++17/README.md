# Content

- [Part I: Basic Language Features](#part-i-basic-language-features)
    - [Chapter 1: Structured Bindings](#chapter-1-structured-bindings)


# Part I: Basic Language Features

## Chapter 1: Structured Bindings

### understand structured bindings
- Structured bindings allow you to decompose an object with multiple members and use these members to initialize multiple entities.
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
    - `std::tuple`, `std::pair`, `std::array`
    - user-defined
        - `std::tuple_size<type>::value`: number of elements
        - `std::tuple_element<i, type>::type`: type of ith element
        - `get<i>(tuple)`: yield value of ith element of object __tuple__
