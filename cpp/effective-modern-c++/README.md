# Content
- [Chapter1: Deducing Types](#chapter-1-deducing-types)
    - [item 1: C++98 template type deduction](#item-1-c98-template-type-deduction)
    - [item 2: auto type deduction (since C++11)](#item-2-auto-type-deduction-since-c11)
    - [item 3: decltype (since C++11)](#item-3-decltype-since-c11)
    - [item 4: viewing deduced types](#item-4-viewing-deduced-types)
- [Chapter 2: auto](#chapter-2-auto)
    - [item 5: prefer auto to explicit type declaration](#item-5-prefer-auto-to-explicit-type-declarations)
    - [item 6: use the explicitly typed initializer idiom when auto deduces undesired types.](#item-6-use-the-explicitly-typed-initializer-idiom-when-auto-deduces-undesired-types)
- [Chapter 3: Moving to Modern C++](#chapter-3-moving-to-modern-c)
    - [item 7: uniform/brace initialization (since C++11)](#item-7-uniformbrace-initialization-since-c11)
    - [item 8: prefer nullptr to 0 or NULL](#item-8-prefer-nullptr-to-0-and-null)
    - [item 9: prefer alias declarations to typedefs](#item-9-prefer-alias-declarations-to-typedefs)
    - [item 10: prefer scoped enums to unscoped enums](#item-10-prefer-scoped-enums-to-unscoped-enums)
    - [item 11: prefer deleted functions to private undefined one](#item-11-prefer-deleted-functions-to-private-undefined-one)
    - [item 12: override, final and member function reference qualifiers (since C++11)](#item-12-override-final-and-member-function-reference-qualifiers-since-c11)
    - [item 13: prefer const_iterators to iterators](#item-13-prefer-const_iterators-to-iterators)
    - [item 14: declare functions noexcept if ther won't emit exceptions](#item-14-declare-functions-noexcept-if-ther-wont-emit-exceptions)
    - [item 15: constexpr](#item-15-constexpr)
    - [item 15.5: overloading and overriding](#item-155-overloading-and-overriding)
    - [item 17: understand special member function generation](#item-17-understand-special-member-function-generation)
- [Chapter 4: Smart Pointers](#chapter-4-smart-pointers)
    - [item 18: `std::unique_ptr`](#item-18-stdunique_ptr)
    - [item 19: `std::shared_ptr`](#item-19-stdshared_ptr)
    - [item 20: `std::weak_ptr`](#item-20-stdweak_ptr)
    - [item 21: prefer `std::make_unique` and `std::make_shared` to direct use of new](#item-21-prefer-stdmake_unique-and-stdmake_shared-to-direct-use-of-new)
    - [item 22: When using the Pimpl Idiom, define special member functions in the implementation file](#item-22-when-using-the-pimpl-idiom-define-special-member-functions-in-the-implementation-file)
- [Chapter 5: Rvalue References, Move Semantics and Perfect Forwarding](#chapter-5-rvalue-references-move-semantics-and-perfect-forwarding)
    - [item 23: `std::move` and `std::forward`](#item-23-stdmove-and-stdforward)
    - [item 24: universal reference vs rvalue reference](#item-24-universal-reference-vs-rvalue-reference)
    - [item 25: return value optimization (RVO)](#item-25-return-value-optimization-rvo)
    - [item 26: avoid overloading on universal references](#item-26-avoid-overloading-on-universal-references)
    - [item 27: alternatives to overloading on universal references](#item-27-alternatives-to-overloading-on-universal-references)
    - [item 28: reference collapsing](#item-28-reference-collapsing)
    - [item 29: understand `std::move` useless cases](#item-29-understand-stdmove-useless-cases)
    - [item 30: perfect forwarding failure cases](#item-30-perfect-forwarding-failure-cases)
- [Chapter 6: Lambda Expressions](#chapter-6-lambda-expressions)
    - [item 31: avoid default capture modes](#item-31-avoid-default-capture-modes)
    - [item 32: use init capture to move objects into closures](#item-32-use-init-capture-to-move-objects-into-closures)
    - [item 33: variadic generic lambda (since C++14)](#item-33-variadic-generic-lambda-since-c14)
    - [item 34: prefer lambda to `std::bind`](#item-34-prefer-lambda-to-stdbind)
- [Chapter 7: The Concurrency API](#chapter-7-the-concurrency-api)
    - [item 35: prefer task-based (using `std::async`) programming to thread-based (using `std::thread`)](#item-35-prefer-task-based-using-stdasync-programming-to-thread-based-using-stdthread)
    - [item 36: `std::async` launch policy](#item-36-stdasync-launch-policy)
    - [item 37: make `std::thread`s unjoinable on all paths](#item-37-make-stdthreads-unjoinable-on-all-paths)
- [Chapter 8: Tweaks](#chapter-8-tweaks)
    - [item 41: pass by value analysis](#item-41-pass-by-value-analysis)
    - [item 42: Consider emplacement instead of insertion](#item-42-consider-emplacement-instead-of-insertion)



# Chapter 1: Deducing Types

## item 1: C++98 template type deduction

Deduction of type `T` depends on `ParamType` and `ArgType`.
```cpp
template <typename T>
void f(ParamType param);

f(arg); // arg is of type ArgType
```

Rules:
```
if ArgType is array or function:
    ArgType does not decay to pointer if ParamType is reference

if ParamType is universal reference and arg is lvalue:
    T, ParamType = lvalue reference
    return
else if pass by value:
    Ignore qualifiers of ArgType
Ignore reference-ness of ArgType
Pattern-match the rest of ArgType against ParamType to determine T
```


## item 2: auto type deduction (since C++11)

The same as C++98 template type duduction:
```
auto == T
ParamType == type specifier (eg. const auto&)
```

One exception:
- template type deduction (and function auto return type deduction since C++14) cannot deduce type of braced list.
- auto type deduction deduced braced list as `std::initializer_list`
    - fixed in C++17: direct list initialization (w/o assignment operation `=`)
    ```cpp
    auto a{42};     // initialize as int
    auto b{42, 1};  // ERROR
    ```

## item 3: decltype (since C++11)
`decltype` echoes back whatever type of the expression passed in.
- for names, return the declaration type of that name
- for lvalue expression other than names, return reference type
```cpp
int x = 0;
// decltype(x) is int
// decltype((x)) is int&
```

Useful for `trailing return type` until C++11. (C++14 supports return
type deduction for all functions and lambdas.)

C++14 supports `decltype(auto)` to use `decltype` rule to deduce auto 
return/variable type.


## item 4: viewing deduced types
- During editing: IDE Editors
  - hover the curosr
  - may be unreliable
- During compiling: Compiler diagnostics
  - deliberately elicit an error message:
    ```cpp
    template <typename T> // declaration only for TD;
    class TD;             // TD == "Type Disaplayer"

    TD<decltype(x)> xType;
    ```
- During runtime: <boost/type_index.hpp>
    ```cpp
    std::cout << boost::typeindex::type_id_with_cvr<T>().pretty_name();
    ```


# Chapter 2: auto

## item 5: prefer auto to explicit type declarations
- it forces initialization when declaring variables
- less typing
- readability issue not as severe as expected, which is demonstrated by
dynamically typed languages such as Python
- more portable (32-bit and 64-bit size_t)
- more efficient
    - for closures, using `std::function` instead of `auto` to store a closure object might involve allocating heap memory to store the closure.
    - in case of mismatches
        ```cpp
        for (const pair<int, int>& p : map<int, int>) {
            // create a copy of pair in each iteration
            // should be const pair<const int, int>&
        }
        ```

## item 6: use the explicitly typed initializer idiom when auto deduces undesired types

- "invisible" proxy classes can cause `auto` to deduce the "wrong" type for an initializing expression.
- Use explicitly typed initializer idiom in this case:
    ```cpp
    auto x = T{y};
    ```
- Why not just `T x = y`, refering to [stackoverflow](https://stackoverflow.com/questions/25607216/why-should-i-prefer-the-explicitly-typed-initializer-idiom-over-explicitly-giv) (still not fully convincing):
    - The modern C++ style prefers the type on the right side
    - `{}` guarantees no narrowing.

# Chapter 3: Moving to Modern C++

## item 7: uniform/brace initialization (since C++11)
- pros:
  - versatile for initializing objects
  - no narrowings
  - immune to C++'s most vexing parse
    ```cpp
    Widget w1();    // function declaration
    Widget w1{};    // call Widget ctor with no args
    ```
- cons:
  - ctor with parameter type `std::initializer_list` would always be matched first if there is possible implicit type conversion from __the types of expressions within the braced list__ to __the element type of the `std::intializer_list`__ even though braced list does not allow narrowing.
    - exception:
      ```cpp
      Widget w{};   // call Widget ctor with no args
      Widget w({}); // call Widget ctor with empty std::initializer_list
      ```

## item 8: prefer nullptr to 0 and NULL
- `nullptr` is of type `std::nullptr_t` and can only be converted to pointer type (and it can only be implicitly converted to boolearn when a test expression is expected)
    ```cpp
    bool b = nullptr;   // ERROR
    if (nullptr) {}     // OK
    ```
- using 0 or NULL can cause trouble in overload resolution:
  - 0 can be implicitly converted to pointer type
  - NULL depends on implementation, might be 0, 0L or nullptr

## item 9: prefer alias declarations to typedefs
- alias declarations (using `using` keyword) support templatization
    ```cpp
    template <typename T>
    using vec = std::vector<T>;
    // vec<int> == std::vector<int>
    ```
- avoids `typename` prefix which is needed for compiler to distinguish between static variable and typename when the type is dependent on a template type parameter. (used in `type_traits`)
    ```cpp
    template <typename T>
    struct vec {
        typedef std::vector<T> type
    }

    template <typename T>
    struct Widget {
        typename vec<T>::type vec1;
    }
    ```

## item 10: prefer scoped enums to unscoped enums
- scoped enum does not leak names
- scoped enum does not allow implicit type conversion
    ```cpp
    enum class Color { white };
    Color i = white;        // ERROR
    int j = Color::white;   // ERROR
    ```
- scoped enum allows forward declaration without declaring underlying type, since the underlying type is defined to be `int` by default
    ```cpp
    enum Color;             // ERROR
    enum Color: int;        // OK
    enum class Color;       // OK, underlying type: int
    ```

## item 11: prefer deleted functions to private undefined one
- allows non member function to be deleted, can be used in:
  - disable implicit type conversions for arguments of function
  - disable specific template instantiations
- Better error message (`deleted` instead of `unable to access`)

## item 12: override, final and member function reference qualifiers (since C++11)
- member function reference qualifers is part of member function signature
- `override` keyword gives you a way to make explicit that a derived class function is supposed to overrride a base class version, it helps you find unexpected mismatches of function signature
- `final` keyword can disable being inherited, used for class or member function

## item 13: prefer const_iterators to iterators
self-explanatory

## item 14: declare functions noexcept if ther won't emit exceptions
- noexcept functions are more optimizable
    - reduce exception handling overhead
        - do not need to handle stack unwinding and destruction of objects in the inverse order they are constructed
    - allow inline expansion
- noexcept is part of the interface and caller can depend on it
- dtor is noexcept by default

## item 15: constexpr
- `constexpr` give a function a feature: when it is called with compile-time constants, it will be executed during compile-time
- `constexpr` is part of a function interface

## item 15.5: overloading and overriding
- function interface: high level abstraction, guarantees and contracts a function offers to its callers
- function signature: syntactic representation, used for function overloading
    - function name
    - parameter types (number and order)
    - qualifiers in member functions (`const`, `volitile`, `&`/`&&`)
- `constexpr`, `noexcept` and return type are parts of function's type instead of signature
- overriding does not allow looser exception specification

|                | Part of Interface | Part of Signature (Affects Overloading) | Affects Overriding |
|----------------|-------------------|----------------------------------------|-------------------|
| const          | Yes               | Yes                                    | Yes               |
| constexpr      | Yes               | No                                     | No                |
| noexcept       | Yes               | No                                     | Yes               |
| ref-qualifiers | Yes               | Yes                                    | Yes               |

## item 17: understand special member function generation
- special member function are the member functions that compiler might generate for you: `ctor, dtor, copy, move` (implicitly `inline`)
- default ctor: generate only if the class contains no user-declared ctors, member would be default-initialized instead of value-initialized
- default copy and move: memberwise copy/`std::move_if_noexcept` for non-static member variables

### __Rule of Three__ extends to __Rule of Five__
  - justification
    - if one kind of memberwise resouce operation is not suitable, others would also be not suitable and need to de defined by users.
    - dtor would also be participating in management of the resource
### General rule of thumb: use `default` and `deleted` keywords explicitly


# Chapter 4: Smart Pointers

## item 18: `std::unique_ptr`
- with custom deleter, size would grow from one word to two
  - if the custom deleter has extensive state, size can grow more
- easy and efficient to convert to `std::shared_ptr`, suitable for factory function

## item 19: `std::shared_ptr`
- typically twice as big as `std::unique_ptr`
  - raw pointer to managed object
  - pointer to a control block:
    - reference count
    - weak count
    - deleter (not part of type as in `std::unique_ptr`)
    - allocator
- performance issues:
  - control block is dynamically allocated on heap
  - increments or decrements of counts are atomic operation
    - `std::move` is preferred if possible since it does not involve counts

## item 20: `std::weak_ptr`
- augmentation to `std::shared_ptr` to prevent `std::shared_ptr` cycles
    ```cpp
    auto spw = std::make_share<Widget>();
    std::weak_ptr wpw{spw};
    auto spw2 = wpw.lock();
    ```

## item 21: prefer `std::make_unique` and `std::make_shared` to direct use of new
- pros
  - improve exception safety
    ```cpp
    int bar();
    void foo(std::shared_ptr p, int i);
    foo(new Widget{}, bar());
    ```
    - if executed in following order and bar() throws an exception, dinamically allocated `Widget` object in step 1 would be leaked, violating __basic exception safety__.
      1. Perform `new Widget{}`
      2. Execute `bar`
      3. Run `std::shared_ptr` ctor
  - more efficient: managed object and control block are allocated together
- cons
  - cannot specify custom deleter
    - side note: we can specify custom allocator using `std::allocate_shared`
  - when object is large and `std::weak_ptr` outlives `std::shared_ptr`

## item 22: When using the Pimpl Idiom, define special member functions in the implementation file

### Pimpl Idiom
- it put private data members into an impl class and store a pointer to the impl class as the member variable
- it decreases build time by reducing compilation dependencies between class clients and class implementations
    ```cpp
    #include "gadget.h" // gadget.h might frequently change

    class Widget {  // in header widget.h
    public:
        Widget();
        ...
    private:
        Gadget g1, g2, g3;
    }
    ```

    ```cpp
    // Alternative using Pimpl Idiom
    #include <memory>

    class Widget {  // in header widget.h
    public:
        Widget();
        ~Widget();
        ...
    private:
        // just declaration, implemented in widget.cpp
        struct Impl; 
        std::unique_ptr<Impl> pImpl;
    }
    ```

### Caveats using `std::unique_ptr` as the impl pointer
- dtor of the primary class must be defined in implementation file because:
  - pointed-to types must be complete when `std::uniqute_ptr` tries to delete its managed object (it call `static_assert` before `delete`)
  - compiler-generated default dtor is implicitly `inline`
  - if we define dtor with definition of impl class in implementation file, it would be able to see the complete type when it is called
- side effects: since dtor is defined, we also need to define `copy` and `move`

# Chapter 5: Rvalue References, Move Semantics and Perfect Forwarding

## item 23: `std::move` and `std::forward`

### Neither `std::move` nor `std::forward` do anything at runtime
### `std::move`
```cpp
template <typename T>
std::remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```
- `std::move` unconditionally casts its argument to an rvalue, it tells
 the compiler the object is eligible to be moved from
- though rvalue reference is an lvalue, when it is returned from a function, it becomes an rvalue (xvalue)
- don't declare an object you want to move from as `const`
    ```cpp
    string(const string& rhs);  // can take const string&&
    string(string&& rhs);   // cannot take const string&&
    ```
- used for move semantics, which makes it possible for compilers to replace expensive copy operations with less expensive moves.
  - in the same way that copy ctor and copy assignments give you control over what it means to copy objects, move ctor and move assignment operators offer control over semantics of moving.
- [understand `std::move` useless cases](#item-29-understand-stdmove-useless-cases)

### `std::forward`
```cpp
template <typename T>
T&& forward(std::remove_reference_t<T>& t) noexcept {
    // forward lvalues as either lvalues of rvalues, depending on T
    return static_cast<T&&>(t);
}

template <typename T>
T&& forward(std::remove_reference_t<T>& t) noexcept {
    // forward rvalues as rvalues
    // and prohibits forwarding rvalues as lvalues
    static_assert(!std::is_lvalue_reference_v<T>);
    return static_cast<T&&>(t)
}
```
- `std::forward` casts its argument to an rvalue only when its argument is an rvalue reference
- used for perfect forwarding, which makes it possible to write function __templates__ that takes arbitrary arguments and forward them to other functions such that the target functions receives exactly the same arguments as were received by the forwarding functions
  - types
  - qualifiers
  - whether they are lvalues or rvalues

## item 24: universal reference vs rvalue reference
- For function template parameter to be a __universal reference__:
  - it must be exactly the form of `T&&` without any other qualifiers
  - there must be type deduction for `T` when the function is called
- otherwise it is an __rvalue reference__
- for variable declared as `auto`, there must be type deduction going on, so `auto&&` suffices to be an __rvalue reference__

## item 25: return value optimization (RVO)
- RVO conditions:
  - the local object is what is being returned (cannot be references or parameters)
  - the local object has the same type as the return type
- when RVO conditions are satisfied, the compiler would construct the return object directly in the memory allocated for the return value (in other words, in the caller's stack frame)
- if not possible to construct the object directly in the caller's stack frame (eg. multiple return statements on lvalues), implicitly `std::move`d would be applied to the returned object

- applied `std::move` to returned rvalue reference or `std::forward` to returned universal reference (that is init as rvalue-ref) can turn a copy construction into a move construction

## item 26: avoid overloading on universal references
- function taking universal references are the greediest functions in C++, they instantiate to match any almost any type of argument
- for overload resolution, when a non-template function and a template instantiation is equally good matches for a function call, the normal function is preferred

## item 27: alternatives to overloading on universal references
- abandon overloading (use different function names)
- use `const T&`
- pass by value
- tag dispatch
    ```cpp
    template <typename T>
    void foo(T&& t) {
        fooImpl(
            std::forward<T>(name),
            std::is_integral<std::remove_reference_t<T>>()  // tag
        );
    }

    template <typename T>
    void fooImpl(T&& t, std::false_type) { ... }

    void fooImpl(int t, std::true_type) { ... }
    ```
- constraining templates via `std::enable_if`
    ```cpp
    template <typename T,
              typename = std::enable_if_t<condition>>
    class Widget {}
    ```

## item 28: reference collapsing

### Context
- template instantiation
  - universal reference is rvalue reference in context of template instantiation
- auto type generation
- `typedef`s and alias declarations

### Rules
- in the contexts above, compilers will convert references to references to a single reference
```
whether the result is rvalue reference == whether both references to be collapsed are rvalue references
```


## item 29: understand `std::move` useless cases

### think about __relationship between objects__

### `std::move` useful cases
- when an object A owns another object B, then A can transfer ownership of B to other object
- the lifetime of object A and object B __can be seperated__ (not necessarily __are seperated__, eg. RAII)
- example
    - `std::unique_ptr` owns its managed object
    - A reader object owns a file handler
    - `std::vector` owns the dynamically allocated array

### `std::move` useless cases
- when object B is part of object A, there is no ownership relationship going on
- lifetime of object A and object B cannot be seperated and are strictly tied together (object B lives on __stack__ inside object A)
- example
    - raw array is part of `std::array`
    - small string optimization (SSO)


## item 30: perfect forwarding failure cases
```cpp
// if the processes of binding the expression of arguments to the function parameters are different, then perfect forwarding fails
func( expr );
std::forward( expr );
```
- braced list
- 0 or NULL as null pointers
- a non-const reference shall not be bound to a bit-field


# Chapter 6: Lambda Expressions
- compile time
  - each lambda expression causes the compiler to generate a unique closure class
- run time
  - a closure object is instantiated from the closure class as the value of lambda expression

## item 31: avoid default capture modes
- default by-reference capture
  - can lead to dangling references
- default by-value capture
  - is susceptible to dangling pointers (especially `this`)
  - static variables are captured by reference

## item 32: use init capture to move objects into closures
```cpp
// scope on the left side of the init:
//      scope of the closure class
// scope on the right side of the init:
//      where the lambda is defined
auto func = [x = std::move(x)]() mutable {
    x = ...;
}
```

## item 33: variadic generic lambda (since C++14)
```cpp
auto f1 = [](auto&&... params) {
    return f2(std::forward<decltype(params)>(params)...);
}
```

## item 34: prefer lambda to `std::bind`

- `std::bind` takes a callable object and returns a new callable one
- using lambda instead of `std::bind` is more efficient in terms of both speed and memory
- lambdas are more readable and more expressive
```cpp
void print(const int i, const std::string &s) {
    std::cout << s << i << '\n';
}

int main() {
    // using std::bind
    using namespace std::placeholders;
    const auto f1 = std::bind(print, 3, _1);
    const auto f2 = std::bind(&print, _2, _1);
    f1("hello"); // print hello3
    f2("world", 3); // print world6

    // lambda alternatives
    const auto f1_lambda = [i = 3](const std::string &s) {
        print(i, s);
    }
    const auto f2_lambda = [](const std::string &s, const int i) {
        print(i, s);
    }
}
```


# Chapter 7: The Concurrency API

## meanings of "threads"

- Hardware threads
    - threads that actually perform computation
- Software threads / OS threads
    - threads managed by operating system across all processes
    - scheduled by os to get executed on hardware threads
    - typically more than hardware threads
- `std::thread`
    - objects in a C++ process
    - a handle to the underlying software threads (if joinable)

## item 35: prefer task-based (using `std::async`) programming to thread-based (using `std::thread`)
- potential problems for thread-based
    - thread exhaustion
        - software threads are limited
        - when no software thread is available, calling `std::thread` will throw a `std::system_error`
    - oversubscription
        - create too many ready-to-run software threads to run on hardware threads, resulting in poor performance
            - context switch overheads
            - high CPU cache miss rates
    - load balancing
        - runtime scheduler is likely to know more than you
- solution: use `std::async` with default launch policy, shifting the thread management responsibility to the implementer of C++ Stand Libray
- cases when you need `std::thread`
    - acceess to the API of the underlying software thread
    - need to and able to optimize thread usage for your application
    - implement threading technology beyond the C++ concurrency API

## item 36: `std::async` launch policy
For a function `f` passed to `std::async` for execution:
- `std::launch::async` start `f` on a different thread and run asynchronously, the returned `std::future` object can be thought of as handle to system threads (as joinable `std::thread` is a handle to system thread)
- `std::launch::deferred` deferred the execution of `f` until either `get` or `wait` is called, when `f` get executed it will execute synchronously
- default (`std::launch::async || std::launch::deferred`) makes execution of `f` unpredictable, it depends on library implementation and the runtime system

## item 37: make `std::thread`s unjoinable on all paths
- `std::thread` is unjoinable when
    - it is default-constructed
    - it has been `std::move`d from
    - it has been `detach`ed
    - it has been `join`ed
- `std::terminate` would be called if the destructor of a joinable `std::thread` is called, because:
    - implicit join-on-destruction can lead to difficult-to-debug performance anomalies
    - implicit join-on-detach can lead to difficult-to-debug undefined behavior
- to handle this, we can use __RAII__, putting `std::thread` in a wrapper

# Chapter 8: Tweaks

## item 41: pass by value analysis
- pros
    - create more maintainable and more readable code, avoid overloading and universal reference
    - create a smaller size of object code
- cons
    - typically costs one extra move for both lvalues and rvalues arguments, this can be problematic:
        - if move is not cheap
        - a chain of function call is involved
        - performance-critical and even avoiding cheap move is important
    - for move-only object, overload is not needed and it makes no sense to use pass-by-value
    - subject to slicing problem, inappropriate for base class parameter types
    - parameters are conditionally copied inside the function body, there can be an extra move/copy if pass-by-value

## item 42: Consider emplacement instead of insertion
- Insertion takes objects to be inserted, while emplacement takes the constructor arguments for objects to be inserted.
- this difference permits emplacement functions to avoid the creation and destruction of temporary objects that insertion functions can necessitate.
- emplacement will almost certainly outperform insertion if:
    - the obejct being added is constructed into the container, not assigned
    - the argument types being passed differ from the element type of the container
    - the container is unlikely to reject the new object as a duplicate
        - permits duplicate
        - most of the values you add will be unique
- even if these followings are all wrong, assume we do not have an existing object in hand, we can say that
    ```
    efficiency of emplacement >= efficiency of insertion
    ```