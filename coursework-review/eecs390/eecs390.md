Reference: [Programming Language Principles and Paradigms, by Amir Kamil](https://eecs390.github.io/notes/)

# Content

- [Foundations](#foundation)
    - [Basic Elements](#basic-elements)
    - [Names and Environments](#names-and-environments)
    - [Memory Management](#memory-management)
- [Functional Programming](#functional-programming)
    - [Higher-Order Functions](#higher-order-functions)
    - [Continuations](#continuations)
- [Data Abstraction](#data-abstraction)

# Foundation

- __imperative programming__: specifies how to do the computation step by step
    - __procedural programming__: computation is structured around procedures (functions)
    - __object-oriented programming__: computation organizeds around "objects" which are instances of "classes"
- __declarative programming__: specifies what result of the computation to get
    - __functional programming__: computation is treated as evaluation of matematical function and avoids changing state and mutable data
- modern languages like C++, Python are multi-paradigm

## Basic Elements

- __grammar__: what phrases are valid
    - __lexical structure__: what constitutes tokens in the language
    - __syntax__: what sequence of tokens constitute valid phrases
- __semantics__: meanings of a valid phrase
    - __expression__: syntatic construct that results in a value
    - __statement__: specifies some action to be carried out
        - an expression can also be a statement
    - __entity__: something that can be named
        - examples:
            - types, functions
            - data object: a location in memory that holds a value
                - variable: name paired with a data object
        - __first-class entity__
            - entities that can:
                - be passed as argument
                - be returned from function
                - assigned to a variable
                - created at runtime

        <img src="first-class-entity.png" width="400">

- __pragmatics__: practical use of valid phrases
    - paradigms
- __implementation__: determines how the actions specified by a meaningful phrase are implemented
    - compiler vs interpreter
    - name resolution
    - memory management

## Names and Environments

- mappings of names to objects are restricted within a __scope__
    - global scope
        - organized by `namespace` in C++
    - local scope
        - introduced by a __block__
            - fundamemtal unit of program organization 
            - two kinds
                - body of a function
                - inline block
- __stack frame__ or __activation record__ is the data structure that keeps track of all the mappings within the function body block
    - __interpreted languages__ _typically_ use __dictionary-based__ implementation
    - __compiled languages__ _typically_ translates name in a frame to an __offset__ from the frame pointer
- name resolution within a __stack frame__
    1. start from inner most scope and proceed to the outer scope if the name is not found
    2. the process is recursively applied to the next outer scope
    3. continue this process until name is found or global scope is reached


## Memory Management


- languages with __value semantics__
    - data object introduced by variable declaration can have:
        - static storage: lifetime spans the entire program
        - automatic storage: lifetime tied to the local scope
        - thread-local storage: lifetime tied to a thread
    - dynamic storage
        - requires explicit operation for creation and destruction
        - __RAII__: uses a resource manager
            - resources lifetime tied to resource manager
                - resources are acquired on resource manager construction
                - released on resource manager destruction
            - resource manager typically has automatic storage duration
            - this way, lifetime of resources is tied to the local scope
- languages with __reference semantics__
    - variable acts as a pointer with automatic dereference
    - assignment to variable changes __which object it refers to__ instead of changing the __underlying object__
    - Python variable and Java variable of `Object` class
        - data objects are allocated on the heap
        - a pointer to the object is created on stack
    - uses __garbage collection__ for deallocation
        - reference counting
            - Python
            - like C++ smart pointers
        - tracing collectors
            - periodically traces out the set of objects in use and collects objects that are not reachable from program code
                - starts from a root set, objects that are not reachable from root set are reclaimed
            - triggered only free space is running slow
                - amortized cost is lower than reference counting
                - but leads to indeterminisitic performance
                

# Functional Programming

## Higher-Order Functions

- higer-order function is function that takes function as parameter or return a function
    - used with lambda function
- one of key features of functional programming
- common computation patterns using higher-order functions
    - __sequence patterns__
        - takes a sequence and a function and apply the function to elements of the sequence, producing a new sequence or value as a result
            - original sequence is left unchanged
        - __map__: applying a unary function to each element in the sequence to produce a new sequence
        - __reduce__: applying a binary function to the first two elements in a sequence, then recursively apply the function to the previous result and the next element in the sequence
            - can be left-associative or right-associative
            - produce a value
        - __filter__: uses a predicate unary function to filter elements in a sequence
            - test true are retained and test false are discarded
            - produce a sequence
        - __any__: uses a predicate unary function and applies it to each successive element in a sequence
            - return the first true result from the predicate
            - return false if all test false
            - produce a value
    - __composition__: compose the above sequence patterns
    - __partial application__: allows to specify some arguments of a function at a different time than the remaining arguments
        - currying: transforming a function that takes `n` arguments into a sequence of `n` unary function

## Continuations

- running program have two kinds of states
    - data
    - control state
- __continuation__ is an abstract representation of control state
    - can be invoked in order to return control to a previous state
    - typically a combination of
        - state of call stacks
        - program counter
- restricted continuations in non-functional languages
    - __subroutine__: just a function being called
        - control transferred from caller when called
        - __continuation__ is implemented by pushing frame pointer and return address on stack
        - return statement can be thought as invoking the __continuation__, it restores caller frame pointer and program counter
    - __coroutine__: a function that can have its execution paused and resumed
        - passing control between __coroutines__ does not involve creation of new activation record like __subroutine__
        - __generator__: a special kind of coroutine
            - when invoked, yield a value and transfer control back to caller without destroying activation record
            - can only transfer control back to caller instead of any other __coroutines__
    - __exception__: control can transfer across multiple stack frames
- first-class continuation in Scheme
    - `call/cc` creates a continuation object representing current control state
    ```scm
    (define error-continuation
      (let ((message (call/cc
                        (lambda (c) c)
                     )
            )
           )
        (if (string? message)
            (report-error message))
        message
      )
    )

    ; continuation object
    (define error error-continuation)
    ```

# Data Abstraction

- __Abstract Data Type (ADT)__: data type that only behavior is defined, not implementation
- object-oriented languages provide means for the following features of ADT:
    - [__encapsulation__](#encapsulation): the ability to bundle together data of an ADT along with the functions that operate on that data
    - [__information hiding__](#information-hiding): the ability to restrict access to implementation details of an ADT
    - [__inheritance__](#inheritance): the ability to reuse code of an existing ADT when defining another ADT
    - [__subtype polymorphism__](#subtype-polymorphism): the ability to use an instance of a derived type where a base type is expected

## __encapsulation__

- member functions / methods
    - non-`static` methods operate on instances of a class, they take in the instance itself as a parameter
        - implicit: C++ `this`
        - explicit: Python `self`
- implementation strategies for access to member
    - access to members can be thought as __sending a message__ to the object
        - languages differ in whether or not the set of messages an object responds to is fixed at compile time
    - __offset-based__: access to data members is translated to a fixed offset into the object at compile time
    - __dictionary-based__: members looked up in a dictionary
        - enable members to be added to an object in run time

## information hiding

- keyword `public`, `private`, `protected`
<img src="./access-control.png">
    - `protected`: C++, C#, and Java prohibit `Derived` from accessing protected member of `Base` through static type `Base`
        ```cpp
        class Base {
        protected:
            int x = 4;
        };

        class Derived : public Base {
        public:
            void foo(Base *b, Derived *d) {
                b->x;   // ERROR
                d->x;   // OK
            }
        };
        ```
- C++ keyword `friend`
    ```cpp
    class Bar { /*...*/ };
    class Foo { 
        friend class Bar;
        // Bar now has access to Foo's private member x
        int x;
    };
    ```
    
## inheritance

- types of inheritance
    - __implementation inheritance__ vs __interface inheritance__
        - __implementation inheritance__ includes __interface inheritnace__
        - a method can be _abstract_ (_pure virtual_) if no implementation is provided
            - a class is _abstract interface_ if all methods are _abstract_
    - C++: `public`, `protected`, `private`
        ```
        accessibility of inherited member = more restrictive of (original accessibility, type of inheritance)
        ```


## subtype polymorphism

### method overriding

- the ability to _override_ a method of base class in derived class is the key to polymorphism
- in some languages, base-class methods that are not overriden but redefined in a derived class are __hidden__ by the definition in the derived class
    - C++
        - non-`virtual` functions
        - `virtual` functions with different signatures
    - in Java, treat as __overload__ instead of __hidden__
    - example: __contravariance__ (see below)
- overriding requires __dynamic binding__, where the actual method to be invoked is determined by the object's __dynamic type__ rather than the __static type__ apparent in the source code
    - to enable __dynamic binding__ in C++, an instance method needs to be declared as `virtual`
        - to help compiler detect whether a method is actually __overriding__, place `override` keyword at the end of method signature
- access to __hidden__ or __overriden__ members
    -
    - C++
        - use scope resolution operator
        - base-class ctor can be explicitly invoked in initializer list
            ```cpp
            struct A { A(int x); };
            struct B : A {
                B(int x) : A(x) {}
            };
            ```
            - if not explicitly invoked, a call to the default ctor of the base class is inserted by compiler
                - `ERROR` if there is no default ctor
            - the base class ctor runs before any other initializers or the body of the derived-class ctor
    - Java and Python
        - uses `super().foo()` to call `foo()` in base class
- __covariance__ vs __contravariance__ patterns
    -
    - __covariance__: return type of an overriding method can be a derived type of the return type in the overridden method
    - __contravariance__: parameter type of an overriding method can be a base type of the parameter type in the overriden method

### implementing dynamic binding

- dictionary-based languages such as Python
    - a sequence of dictionary lookups at runtime
        1. search in the dictionary of the object itself
        2. search in the dictionary of the class
        3. search in the dictionary of base class
- offset-based languages such as C++
    - store pointers to `virtual` methods in a data strcutre called __virtual table__ (__vtable__)
    - the storage of an object of type `A` contains a pointer to the __vtable__ for class `A` as the first item (if `A` has `virtual` methods)
        ```cpp
        struct A {
            int x;
            double y;
            virtual void a();
            virtual int b(int i);
            virtual c(double d);
            void f();
        };

        struct B : A {
            int z;
            char w;
            virtual void d();
            virtual double e();
            virtual int b(int i);
            void f();
        };
        ```
        <img src="./layout-derived.png">

        1. look up the member in the __static type__ of the receiver
        2. if non-`virtual`, generate a direct dispatch
        3. if `virtual`, determine its offset in the __vtable__ of the static type
            - two extra dereferences:
                ```cpp
                A *aptr = new B;
                aptr->b();

                // extract vtable pointer from start of object
                vtable_ptr = aptr-><vtable>;
                // index into vtable at statically computed offset for b
                func_ptr = vtable_ptr[1];
                // call function, pass the implicit this parameter
                func_ptr(aptr);
                ```