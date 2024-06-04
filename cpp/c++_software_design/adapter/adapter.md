## [Index](../c++_software_design.md)

# Adapter Design Pattern

## Analysis

- Adapter converts the interface of a class into another interface clients expect and lets classes work together that couldn’t otherwise because of incompatible interfaces.
    - it also applies to function, since free function overloading is a powerful compile-time abstraction mechanism
- strengths
    - __nonintrusive__: one of the greatest strengths
    - __SRP and OCP__: separates the concerns of the interface from the implementation details
- shortcomings
    - makes it very easy to combine things that do not belong together, like adapts a turkey into a duck, it’s very important that you consider the expected behavior and check for LSP violations when applying this design pattern

## Object Adapters vs Class Adapters

- __object adapter__: store an instance of the wrapped type
    - __Comparison with Strategy__:
        - structurally similar to Strategy, but in Adapter, the wrapped type specifies not just a single aspect of the behavior, but most of the behavior or even all of it.
        - The intent is also totally different:
            - the primary focus of an Adapter is to standardize interfaces and integrate incompatible functionality into an existing set of conventions;
            - while on the other hand, the primary focus of the Strategy design pattern is to enable the configuration of behavior from the outside, building on and providing an expected interface.
            - Also, for an Adapter there is no need to reconfigure the behavior at any time.
    - __Flexibility__
        - given that the wrapped type is part of an inheritance hierarchy, you could store a pointer to the base class of this hierarchy. This would allow you to use the object adapter for all types that are part of the hierarchy, giving the object adapter a considerable boost in flexibility.
        - you can also makes the wrapped type to be template parameter like `std::stack`
    - object adapter uses composition over inheritance and should be preferred
- __class adapter__: inherit from the adapted type