# Name Lookup and Overload Resolution

- [name lookup](#name-lookup)
- [overload resolution](#overload-resolution)
- [process after overlaod resolution](#process-after-overload-resolution)
- [reference](#reference)

# name lookup

- name lookup in regular function consider only visible candidates from its definition context

## qualified name lookup
- name appears on the right side of scope resolution operator `::`
- recursive
    ```cpp
    entity qualified_name_lookup(entity_name name) {
        auto [left, right] = name.split_by_rightmost_scope_resolution_op();
        if (leftside.qualified())
            scope = qualified_name_lookup(left);
        else    
            // empty leftside results in global namespace scope
            scope = unqualified_name_lookup(left);
        return scope.name_lookup(right)
    }
    ```

## unqualified name lookup

- examines from inner scopes to outer scopes, scope traversing stops when it finds an entity with same name __(regardless of its category)__, so names in inner scope __hide__ same names in outer scope
    - scope from inner to outer:
        - inner block -> outer block
        - derived class -> base class
        - inner namespace -> outer namespace -> global namespace
    - if there are multiple base classes with entities of same names, it results in __ambiguity in request for member__
    - tips
        - use `using`-declarations to bring in the names from outer scope into current scope (whether the scope is `class` or namespace)
        - make namespaces flat and shallow
- __ADL (argument dependent lookup)__: for unqualified function name in a function call expression, the function name is also looked up in the namespace where the type of __argument (not parameter)__ is __declared__
    - outer namespaces are not included
    - the __type aliases__ are fully resolved and expanded to their source types before the list of namespaces to ADL search are chosen
    - __hidden friend__
        - __definition__: friend function declared and defined inside of a class and taking this class type as an arguement
        - such function can be found only through the ADL (through the argument of the same class type, conversion operator does not work)
        - friend functions are not a part of the candidate set for arguments of other types which means they make the name lookup and overload resolution process faster
    - __customization point object__
        - C++20's way to implement [customization point](https://stackoverflow.com/questions/76732333/what-is-the-clear-definition-of-customization-point), and solves customization dispatch (referred from [Barry's C++ Blog](https://brevzin.github.io/c++/2020/12/19/cpo-niebloid/))
        - customization interface entry point (like `std::ranges::swap`) cannot be found through ADL
            ```cpp
            namespace std::ranges {
                struct __swap_impl {
                    template <class T>
                    requires /* ... */
                    void operator()(T& t1, T& t2) {
                        using std::swap;
                        swap(t1, t2);
                    };
                };
                inline constexpr auto swap = __swap_impl{};
            }
            ```
        - customization point function found only via ADL
            ```cpp
            namespace ns {
                struct S {};
                void swap(S&, S&);  // found through ADL by customization point object
            }
            ```

## two-phase name lookup in function template

- __Declaration Phase__:
    - During the declaration phase, the template parameters are not considered, and name lookup is performed based on the template's definition context.
    - This phase does not consider the template arguments or perform any type-dependent name lookup.
- __Instantiation Phase__:
    - The instantiation phase occurs when the function template is instantiated with specific template arguments.
    - In this phase, name lookup is performed again, taking into account the actual template arguments and any type-dependent expressions.
    - The instantiation phase considers names that are visible at the point of instantiation, including names that became visible after the template definition.
    - If a name is not found in the instantiation context, the lookup falls back to the declaration context.

```cpp
template <class T>
void doSomething(T t);      // (1)

template <class T>
void func(T t) {
    doSomething(2);     // name lookup in declaration phase, resolve to (1)
    doSomething(t);     // name lookup might involve ADL and is dependednt on type T
}

namespace NS {
    struct S {};
    void doSomething(S);    // (2)
}

func(NS::S{});  // Instantiation Phase: doSomething(t) resolves to (2)
```

# overload resolution

Process of selecting the most appropriate overload

## overload vs override

- definition of polymorphism
    - having many forms
    - type-based dispatch
- __compile-time/static polymorphism__
    - function and __primary__ function templates overloading
    - dispatch based on __static type__ (of argument specifically)
    - for functions with the same name and visible from the same scope to be overloaded:
        - different parameter lists, _or_
        - if function template, different template constraints (with C++20 concept)
- __run-time polymorphism__
    - member function overriding
    - dispatch based on __runtime type__ of object
    - used with inheritance and `virtual` member functions, unrelated to the subject of overload resolution

## rationale

- avoids long names like `doFunctionStr`, `doFunctionPairIntStr`, ...
    - delegate this name mangling job to compiler
- for functions doing roughly equivalent but different things for different types/concepts
    - if applying the same operations, use function template instead


## Process

- __name lookup__
    - find a list of candidates (functions and function templates) in 
        - ADL __namespaces__ and
        - innermost __scope__ that has an __entiy__ with the same name
- __template argument deduction__
    - for all candidate function templates, deduce all template parameters based on given template arguments
    - __Substitution failure is not an error__ and corresponding function template candidate is simply removed from the candidate set
- __remove all not viable candidates__
    - too many/few arguments
    - no implicit conversion from argument type to parameter type
- __rank the remaining candidates (most complex part)__
    - exactly one function with highest rank -> it is chosen
    - more than one function with highest ranks -> use __tie breakers__
    - __ranks (single parameter)__
        1. __exact match (including trivial conversion)__
            - including array-to-pointer, function-to-pointer
            - by-value and by-reference
            - value categories transformations
            - cv qualifier adjustment
        2. __promotion__
            - integral promotion: `short`, `char`, `bool` to `int`
            - float-point promotion: `float` to `double`
        3. __other standard conversion__
            - all other scalar types conversion
                - int-to-float
                - derived pointer/ref to base pointer/ref
                - pointer to `void *`
                - ...
        4. __implicit conversion involving `class` type__
    - __tie breakers__
        - reference/pointer -> less cv-qualification
        - derived ptr/ref to base ptr/ref -> closer base ptr/ref
        - template and non-template -> non-template
        - template with constraints -> more constrained (since C++20)
        - ...
    - for serveral arguments, if one function is considered better for at least one argument and equally good for all other arguments, the function is selected as best match, else the function is ambiguous

# process _after_ overload resolution

1. __access labels__
    - if it is member function, check if it is __access violation__
    - ___note:___ everything inside a class is visible, `public`, `protected` adn `private` are merely access lables
2. __function template specialization__
    - if the best match results from a template, choose the final function from the set of all specializations of the selected function template
    - ___note:___ prefer function overloading to function template specialization
3. __virtual dispatch__
    - if the best match is a non-`final` virtual function, look up virtual table for the dispatch
4. __deleting function__
    - check if function `= delete`

# Reference

- [Back to Basics - Name Lookup and Overload Resolution in C++ - Mateusz Pusz - CppCon 2022](https://www.youtube.com/watch?v=iDX2d7poJnI)
- [Back To Basics: Overload Resolution - CppCon 2021](https://www.youtube.com/watch?v=b5Kbzgx1w9A)
- [Calling Functions: A Tutorial - Klaus Iglberger - CppCon 2020](https://www.youtube.com/watch?v=GydNMuyQzWo)
- https://en.cppreference.com/w/cpp/language/overload_resolution#Candidate_functions