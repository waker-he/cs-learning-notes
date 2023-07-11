
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
    ```cpp
    template<>
    const char* min(const char* pa, const char* pb) {
        return (strcmp(pa, pb) < 0) ? pa : pb;
    }

    template<>
    struct is_int<int> {
        static const bool value = true;
    };
    ```
- partial specialization
    ```cpp
    template<T>
    struct isPointer {
        static constexpr bool value = false;
    };

    template<T>
    struct isPointer<T *> {
        static constexpr bool value = true;
    };

    template<T>
    inline constexpr bool isPointer_v = isPointer<T>::value;
    ```