# Object Lifetime

## Start & End

- the lifetime of an object of type `T` begins when
    - storage with proper size and alignment for type `T` is obtained
    - if the object has __non-vacuous initialization__, its initialization is __complete__
        - __non-vacuous initialization__: class type and it is intialized by a ctor other than a trivial default ctor [(link to definition of trivial special member function)](../cppcon/type-categories.md#trivial-special-member-function)
        - if ctor throws, initialization is not complete and lifetime is not considered "has begun", so the dtor will not be called when its storage is released 
- the lifetime of an object of type `T` ends when
    - if `T` has a non-trivial dtor, its dtor call __starts__
    - else, its storage which the object occupies is released, or is reused by another object


## Lifetime of Temporary Object 

Temporary objects are destroyed as the last step in evaluating the full-expression that (lexically) contains the point where they were created.
```cpp
Foo{}, doSomething(); // ~Foo() is called after doSomething() is finished
```
- this might not work with range-based for loop (until C++23), use range-based for loop with init (since C++20) for workaround:
    ```cpp
    for (auto coll = foo(); auto& x : coll.items()) { /*...*/ }
    ```

### Lifetime Extension

- lifetime of a temporary object `T` will be extended when
    - prvalue of `T` or its data member (must be accessed through field access operator `.`) is used to initialize an lvalue reference-to-`const` or rvalue reference



## Reference

- [C++ Object Lifetime: From Start to Finish - Thamara Andrade - CppCon 2023](https://www.youtube.com/watch?v=XN__qATWExc&list=WL&index=14&t=39s)
- [abseil / Tip of the Week #107: Reference Lifetime Extension](https://abseil.io/tips/107)