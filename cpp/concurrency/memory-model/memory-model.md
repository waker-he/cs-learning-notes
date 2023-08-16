# References

- [C++ and Beyond 2012: Herb Sutter - atomic Weapons 1 of 2](https://www.youtube.com/watch?v=A8eCGOqgvH4&t=1522s)
- [C++ and Beyond 2012: Herb Sutter - atomic Weapons 2 of 2](https://www.youtube.com/watch?v=KeLBd2EJLOU)
- [CppCon 2017: Fedor Pikus “C++ atomics, from basic to advanced. What do they really do?”](https://www.youtube.com/watch?v=ZQFzMfHIxng)
- [CppCon 2015: Michael Wong “C++11/14/17 atomics and memory model..."](https://www.youtube.com/watch?v=DS2m7T6NKZQ&t=2670s)
- [cppreference std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)
- [C++ standard [atomics.order]](https://eel.is/c++draft/atomics.order#def:coherence-ordered_before)

# Content

- [Motivation for defining memory model](#motivation-for-defining-memory-model)
- [Definition and Some Key Concepts](#definition-and-some-key-concepts)
- [`std::memory_order`](#stdmemory_order)

# Motivation for defining memory model

- two reasons: optimizations and concurrency
    - your program will not be executed as you wrote
    - in order to optimize the performance of execution, "the system" (compiler, processor and cache) will "transform" your program
        - __transformations__: reorder, invent, remove instructions
        - <img src="./transformations.png" width="300">
    - these optimizations are fine in single-threaded context, "the system" will provide an illusion of __sequentially consistency (SC)__
        - __sequentially consistency (SC)__: the processor perform the operations in order exactly as specified by the program
    - but things get messy when concurrency is involved, besides straightforward consequence like __cache incoherence__, there are many other subtle problems that can happen, see the following examples:

## register allocation (by compiler)
- for this general pattern:
    ```
    if (cond) lock x
    ...
    if (cond) use x
    ...
    if (cond) unlock x
    ```
- if we have:
    ```
    if (cond) x_mutex.lock()
    for (...) {
      if (cond) ++x;
      ...
    }
    if (cond) x_mutex.unlock()
    ```
- a kind of transformation for the `for-loop` will be:
    ```
    r1 = x
    for (...) {
        if (cond) ++r1
    }
    x = r1;     // oops: write is not conditional
    ```

## speculation (by processor)
- for the same above pattern, if processor speculates that `cond` is true:
    ```cpp
    if (cond) x = 42;
    // transform to:
    r1 = x;             // read what's there
    x = 42;             // oops: optimistic write is not conditional
    if (!cond) x = r1;  // write back if guessed wrong
    ```

## store buffer (processor/cache)

- even if "the system" do not do any transformations except for using a __store buffer__:
    <img src="./store_buffer.png" width="500">


# Definition and Some Key Concepts

- __memory model__ is a __contract between the programmer and the system (comprising the compiler, processor, and cache)__
    - it specifies the rules and guarantees around how memory operations will appear to execute, relative to each other, __in a concurrent environment__
    - the model defines if, and under what conditions, the system provides the illusion of __sequentially consistent memory accesses__
- C++11's default memory model is __Sequentially Consistent for Data Race Free (SC-DRF) programs__
    - this means that if a program is __data race-free__, then the system will provide the illusion that all memory operations are executed in the program order
    - otherwise, it is __undefined behavior__
- __data race__: a program is said to have __data race__ if two or more threads access the same memory location and:
    - at least one of the thread performs a __write operation__
    - at least one operation is not __atomic__
    - without __synchronization__
- __synchronize-with__:
    - operation A in one thread __synchronizes with__ operation B in another thread if A is a __release operation__ and B is an __acquire operation__ that is __paired with A__
        - `mutex::unlock()` is a __release operation__ and `mutex::lock()` is an __acquire operation__
            - they pair with each other when operating on the __same mutex__
    - to achieve __SC-DRF memory model__, __release operation__ and __acquire operation__ act as __memory barrier__
    - __memory barrier__ prevents code from being reordered across it in one direction or both directions
        - it requires cooperation with software (compiler) and hardware (processor and cache)
        - memory operations before a __release operation__ cannot be reordered to be after the __release operation__
            - __release__ is considered as publishing data that has been written to other threads
        - memory operations after a __acquire operation__ cannot be reordered to be before the __acquire operation__
            - __acquire__ is considered as acquiring data that is published by other threads
            ```cpp
            // x = 2 cannot be reordered before mut.lock()
            mut.lock();
            x = 2;
            mut.unlock();
            // x = 2 cannot be reordered after mut.unlock()
            ```
            <img src="./memory_barrier.png" width="500">
    - memory synchronization __actively works against__ important modern hardware optimizations
        - => want to do as little as possible
- __happen-before__:
    - A happens before B if:
        - same thread: A is sequenced-before B
        - different threads: A synchronizes with B
        - A happens before C and C happens before B

# `std::memory_order`

- defines how memory operations can be reordered around an __atomic__ operation
- `std::memory_order_relaxed`: does not form a memory barrier
- `std::memory_order_acquire`: memory operations after it cannot be reordered before it
- `std::memory_order_release`: memory operations before it cannot be reordered after it
- `std::memory_order_acq_rel`: form a full fence
- `std::memory_order_seq_cst`: form a full fence and there is a single global ordering between all `std::memory_order_seq_cst` operations
    - "single global ordering" is an ordering of operations that is agreed by all threads, see the following example:
        ```cpp
        #include <thread>
        #include <atomic>
        #include <cassert>
        
        std::atomic<bool> x = {false};
        std::atomic<bool> y = {false};
        std::atomic<int> z = {0};
        
        void write_x()
        {
            x.store(true, std::memory_order_seq_cst);
        }
        
        void write_y()
        {
            y.store(true, std::memory_order_seq_cst);
        }
        
        void read_x_then_y()
        {
            while (!x.load(std::memory_order_seq_cst))
                ;
            if (y.load(std::memory_order_seq_cst)) {
                ++z;
            }
        }
        
        void read_y_then_x()
        {
            while (!y.load(std::memory_order_seq_cst))
                ;
            if (x.load(std::memory_order_seq_cst)) {
                ++z;
            }
        }
        
        int main()
        {
            std::thread a(write_x);
            std::thread b(write_y);
            std::thread c(read_x_then_y);
            std::thread d(read_y_then_x);
            a.join(); b.join(); c.join(); d.join();
            assert(z.load() != 0);  // will never happen
        }
        ```
        - there is no happen-before relation between `write_x` and `write_y`
        - without `std::memory_order_seq_cst`, thread c and thread d might see different views of the ordering of `write_x` and `write_y`, and it is possible that thread c sees `write_y` then `write_x`, thread d sees `write_x` then `write_y`
    - default `std::memory_order` for all atomic operations
    - it is often used when multiple atomic variables are involved
        - for release/acquire pair, only one atomic variable is involved
    - avoid mixing `std::memory_order_seq_cst` with other relaxed atomic operations, otherwise it can produce surprising results
- use `std::atomic` to __synchronize__:
    - for an atomic load in thread A and an atomic store in thread B:
        - they operate on the same variable
        - atomic load is an __acquire operation__
        - atomic store is a __release operation__
        - atomic load in thread A reads the value that is written by the atomic store in thread B
    - `std::memory_order_release`, `std::memory_order_acq_rel`, `std::memory_order_seq_cst`: atomic store with these `std::memory_order` is a __release operation__
    - `std::memory_order_acquire`, `std::memory_order_acq_rel`, `std::memory_order_seq_cst`: atomic store with these `std::memory_order` is an __acquire operation__

## usage in double-checked locking pattern (DCLP)

```cpp
// singleton initialization
std::atomic<Widget*> Widget::ptrInstance{nullptr};
Widget* Widget::instance() {
    Widget* ptr = ptrInstance.load(std::memory_order_acquire);  // 1
    if (ptr == nullptr) {       // first check
        std::lock_guard<std::mutex> lg{mutW};
        ptr = ptrInstance.load(std::memory_order_relaxed);      // 2
        if (ptr == nullptr) {   // second check
            ptr = new Widget();
            ptrInstance.store(ptr, std::memory_order_release);  // 3
        }
    }

    // using temporary variable ptr to avoid an extra atomic load when return
    return ptr;
}
```
- for atomic load `1`, if it uses `std::memory_order_relaxed`, it will not synchronize with release operation `3`
    - it can see that ptrInstance is not `nullptr` but the memory it points to can still be garbage
    - in this case, it will return a pointer to the `Widget` object that is in a "partially-constructed" state