## [Index](../c++_software_design.md)

# Understanding Design Patterns

- A design pattern is a proven, named solution, which expresses a very specific intent. It introduces some kind of abstraction, which helps to decouple software entities and thus helps to manage the interaction between software entities.
    - __Has a name__: The name of a design pattern allows us to communicate on a very high level and to exchange a lot of information with very few words.
    - __Carries an intent__: If you use the name of a design pattern, you can express your intent concisely and limit possible misunderstandings. You implicitly state what you consider to be the problem and what you see as a solution.
    - __Introduces an Abstraction__: A design pattern always provides some way to reduce dependencies by introducing some kind of abstraction. This means that a design pattern is always concerned with managing the interaction between software entities and decoupling pieces of your software.
        - key difference from __implementation pattern__: implementation pattern is recurring solution for a specific problem about impelementation details and is often language-specific
    - __Has been proven__
- __Tools instead of Goals__: as with SOLID principles
- __Not paradigm centric__: Design patterns are not limited to object-oriented programming or runtime polymorphism. But while most design patterns are not paradigm centric and their intention can be used in a variety of implementations, some are more specific.
    - There are some design patterns whose intent is targeted to alleviate the usual problems in object-oriented programming (e.g., the Visitor and Prototype design patterns). And of course there are also design patterns focused on generic programming (e.g., CRTP)
- Use the name of a design pattern to express intent and improve readability.
    ```cpp
    template< class InputIt, class T, class BinaryOperation >
    constexpr T accumulate( InputIt first, InputIt last, T init, 
                            BinaryOperation op );

    template< class InputIt, class T, class BinaryReductionStrategy >
    constexpr T accumulate( InputIt first, InputIt last, T init, 
                            BinaryReductionStrategy op );
    ```