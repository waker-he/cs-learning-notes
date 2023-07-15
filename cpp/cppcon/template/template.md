
- [Template Basics](#template)
    - [Definition](#definition)
    - [Template Parameters](#template-parameters)
    - [Related Terms](#related-terms)
- [Template Metaprogramming](#template-metaprogramming)
- [Concepts (since C++20)](#concepts-since-c20)

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
    - cannot specify default template arguments
        - uses default template arguments in primary template
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
    - if substitution fails during template instantiation (__get ill-formed declaration__), it does not immediately result in an error, compiler will search for other overload candidates, if cannot find one, ERROR
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


# Concepts (since C++20)

- C++20 feature _concepts_ allows specifying ___constraints___ for generic code, with the keyword `requires` and `concept`

## `requires` clause, `concept` and `requires` expression

- `requires` clause
    - used to restrict the availability of template
        ```cpp
        template <class T>
        requires COMPILE_TIME_BOOLEAN
        void foo(T arg);
        ```
- `concept`
    ```cpp
    template <class From, class To>
    concept convertible = std::is_convertible_v<From, To>;
    ```
    - can be thought as `constexpr` boolean variable template
    - three key differences from actual `constexpr` boolean variable template
        1. can be used as __type constraints__
            ```cpp
            template <Integral T>
            void foo();

            // equivalent to:
            template <class T>
            requires Integral<T>
            void foo();
            ```
            - can also be used to constrain `auto`
                ```cpp
                // convertible automatically takes
                // auto type as the first template argument
                convertible<int> auto i = foo();
                ```
        2. concepts __subsume__
            ```cpp
            template <class T>
            requires std::movable<T>
            void foo(T arg);    // (1)

            template <class T>
            requires std::copyable<T>
            void foo(T arg);    // (2)

            // results in ambiguity if using type traits
            // in the requires clauses
            // calls (2) since concept in requires clause(2)
            // subsumes concept in (1)
            foo<int>(0);    
            ```
            - order matters for subsume to take place, be careful when defining commutative concepts
        3. is __prvalue__, cannot take its address

- `requires` expression
    - evaluates to compile-time `bool` value
    - facilitates definition of `concept`
    - used to specify requirements on one or more template parameters
        1. __simple requirements__: expressions that have to be valid
        2. __type requirements__: type names that have to be defined
        3. __compound requirements__: what types an expression yields and whether the expression can throw
        4. __nested requirements__: use syntax of `requires` clause to speicify the compile-time boolean result an expression should yield
            - used `concept` here is not subsumed
        ```cpp
        template <class T1, class T2>
        concept c = requires(T1 p /*optional parameter list*/) {
            // 1. operator[] has to be supported for T1
            p[0]; 

            // 2. T1 and T2 have to have a common type
            //  the preceding typename is necessary
            typename std::common_type_t<T1, T2>;

            // 3. note that covertible_to and is_same_v takes 
            //  the yielding type as first argument
            { p == p }noexcept -> std::convertible_to<bool>;
            { *p } -> std::is_same_v<int>;

            // meaningless, always valid
            std::is_const_v<T>;
            typename std::remove_const_t<T>;
            // 4. requires T to be non-const
            requires !std::is_const_v<T>;
        };
        ```

## notes
- advantages over SFINAE for specifying constraints
    - more readable generic code
    - more readable and understandable error message when constraints are broken
        - show which requirement is broken
        - instead of showing internals of substitution process
    - less error-prone and easier to implement
- `concept`: between type and `auto`
    - type specifies the interface and layout in the memory
    - `concept` only specifies the interface
        - more generic than concrete type
        - makes the interface of generic functions clear and understandable
            - make generic code more like ordinary
    - `auto` does not specify anything at all