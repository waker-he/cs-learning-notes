
- [Template Basics](#template)
    - [Definition](#definition)
    - [Template Parameters](#template-parameters)
    - [Related Terms](#related-terms)
- [Template Metaprogramming](#template-metaprogramming)

# Template

## Definition
_thing_ template is a parametrized description of a family of _things_
- it is a recipe to make _things_
- _thing_ can be:
    - class (C++98/03)
    - function (C++98/03)
    - member function (C++98/03)
    - type alias (C++11)
    - variable (C++14)
    - lambda (C++20)

## Template Parameters
- type parameters(`typename`/`class`)
- Non-type template parameter (NTTP)
    - integral and enumeration
    - pointers to objects/functions/members with static storage
    - lvalue reference
    - `std::nullptr_t`
- template
    ```cpp
    template <class T, template<class U, class A = std::allocator<U>> class Container>
    struct Adaptor {
        Container<T> my_data;
    };
    ```

## Related Terms

- Specialization 
    - the __concrete entity__ resulting from substituting template arguments for template parameters
    - the _thing_ to make
- Template Instantiation (implicit specialization)
    - __process__ of __compiler__ substituting template arguments for template parameters to make the _thing_(specialization)
    - instantiation is a synonym for _compiler-generated_ specialization
    - can be done in two ways:
        - implicit/on-demand/automatic
            - compiler sees the use of speicialization
            - compiler decides where, when and how much of the specialization to create
            - only instantiates members that are used
        - explicit
            - programmer decides where and when the instantiation occurs
            - instantiates _all_ members
            - one definition rule applies
            ```cpp
            // in source file
            template class vector<foo>; // definition
            // in header file
            extern template class vector<foo>;  // declaration
            ```
- explicit specialization
    - __user__-provided implementation of a template with all template parameters fully substituted
    - caveats for __function template__:
        - avoid using explicit specialization for function templates, use ordinary function with the same name instead
        - explicit specialization is __NOT__ a candidate for function overload resolution, for following code:
            - (b) is an explicit speicalization of (a)
            - (a) and (c) compared
            - (c) is chosen since it is more specialized
            - if (b) is declared after (c), (b) becomes explicit specialization of (c), then (b) will get called
            ```cpp
            template <class T> void foo(T t);    // (a)
            template <> void foo(int* t);        // (b)
            template <class T> void foo(T* t);   // (c)

            int *ptr = nullptr;
            foo(ptr);           // calls (c)
            ```
- partial specialization
    - used for class template and variable template
    - examples
    ```cpp
    template <class T, class U>
    struct S;
    
    // more specialized
    template <class T>
    struct S<T, T>;

    template<class T>
    struct isPointer {
        static constexpr bool value = false;
    };

    // more specialized
    template<class T>
    struct isPointer<T *> {
        static constexpr bool value = true;
    };
    ```

# Template Metaprogramming

- Metaprogramming
    - writing computer programs that
        - treat other programs (or themselves) as data
        - do work at compile time that would otherwise be done at runtime
- C++ template metaprogramming uses __template instantiation__ to drive compile-time evaluation
    - purpose
        - source code flexibility
        - runtime performance
- metafunctions
    - class template
    - called by requesting public data members
    - use metafunction calls (possibly recursive), inheritance and aliasing to factor commonalities
    - pros over `constexpr` function
        - can have type result
        - can have public members and `constexpr` functions
    - conventions
        - type result should be named `type`
        - value result should be named `value`
    - check [is_one_of.cpp](./is_one_of.cpp)
- SFINAE
    - substitution failure is not an error
    - if substitution fails during template instantiation, it does not immediately result in an error, compiler will search for other overload candidates, if cannot find one, ERROR
    - used to direct overload resolution
    ```cpp
    // decltype operand is never evaluated
    template <class T>
    using copy_assignable = decltype(declval<T&>() = declval<const T&>());
    // change `const T&` to `T&&` for move_assignable

    // default `void` is essential
    template <class T, class = void>
    struct is_copy_assignable : false_type {};

    // more specialized, considered first for overload resolution
    // substitution failure if not copy assignable
    template <class T>
    struct is_copy_assignable<T,
                              void_t< copy_assignable<T> >
                              >
            : is_same<T&, copy_assignable<T>> {};
    ```