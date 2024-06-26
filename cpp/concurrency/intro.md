## [Index](../README.md)

# Concurrency Overview

- [Concurrency vs. Parallelism](#concurrency-vs-parallelism)
- [Approaches to Concurrency: multi-processing vs. multi-threading](#approaches-to-concurrency)
- [Reasons to use or not use concurrency](#reasons-to-use-or-not-use-concurrency)
- [Race conditions](#race-conditions)
- [Deadlock](#deadlock)
- [Monitor](#monitor)

## Concurrency vs. Parallelism

### Parallelism

- The term __Parallelism__ refers to techniques to make programs faster by performing several computations at the same time.
    - This requires __hardware concurrency__, which refers to hardware's property of having multiple __hardware threads__.
    - __hardware threads__: a core within a processor, or if the core can run multiple hyperthread simultaneously, one hardware thread == one hyperthread
- Graphic computations on a GPU are parallelism.
- A key problem of parallelism is to reduce data dependencies in order to be able to perform computations on independent computation units with minimal communication between them. To this end, it can even be an advantage to do the same computation twice on different units.

### Concurrency

- A condition that exists when at least two threads are making progress, they can run at the same time but not necessarily.
- __Hardware concurrency__ is not required, multi-tasking operating system can create an illusion that these threads are running at the same time by time-slicing on a single processing unit.
- concurrency is a property of a program or system (and parallelism as the run-time behaviour of executing multiple tasks at the same time)  – 
Adrian Mouat on stackoverflow

## Approaches to Concurrency

### concurrency with multiple processes

- communicate through inter-process communication channels (setup through OS)
- pros
    - easier to write safe concurrent code
    - can run on distinct machines connected over a network
- cons
    - IPC is often complicated to setup or slow, or both
    - starting a process consumes more resources and overhead than thread
    - C++ Standard does not provide intrinsic support for communication between processses, application using multiple process concurrency will have to rely on platform-specific API

### concurrency with multiple threads
- pros
    - easier for communication (through the shared address space)
    - less overhead for launching and context switching
        - launching requires a new stack section instead of the whole address space
        - switching between threads in the same process do not need to change `page_table_base_register` or invalidate cache using virtual memory
    - C++ Standard (since C++11) provides facilities
- cons
    - harder to write safe concurrent code
    - can't run on distinct machines

## Reasons to use or not use concurrency

- reasons for not using concurrency
    - intellectual cost to writing and maintaining multithreaded code, and the additional complexity can also lead to more bugs
- reasons to use concurrency
    - separation of concerns
    - performance (with __parallelism__)
        - __task parallelism__: divide a single task into parts and run each in parallel
        - __data parallelism__: performing the same operation on multiple sets of data in parralel
        - note the performance cost with context switching and launching a thread
            - match number of threads with number of __hardware threads__ (avoid _oversubscription_)
                - `int numHardwareThreads = std::thread::hardware_concurrency()`
            - do not start a new thread for a trivial task

## Race conditions

- __race condition__: a situation in the context of concurrency, where the outcome depends on the relative ordering of execution of operations on two or more threads; the threads race to perform their respective operations.
- a race condition is benign if all possible outcomes are acceptable
- race condition is __time-sensitive__, it might disappear entirely when app runs under debugger
- __problematic__ race condition: race condition that can lead to broken invariants or unexpected behaviors
    - term __race condition__ usually means __problematic race condition__
    - __data race__ is a kind of problematic race condition
    - problematic race condition might not be caused by data race
        - a simple example will be a mutex-protected stack and we perform `empty()` and `pop()` separately
- ways to deal with problematic race conditions
    1. associate your data structure with a __mutual exclusion mechanism__ to ensure that only the thread performing a modification can see the intermediate states where the invariants are broken, from the point of view of other threads, the modification is atomic (either haven't started or have completed)
    2. another option is to modify the design of your data structure and its invariants so that modifications are done as a series of indivisible changes, each of which preserves the invariants. This is generally referred to as __lock-free programming__ and is difficult to get right
    3. __software transactional memory (STM)__: not directly supported by C++ yet

## Deadlock

- cyclical waiting for resources which prevents progress
    - problematic race conditions comes from insufficient constraints, deadlock comes from overconstraints
    - can occur with any synchronization construct that can lead to a wait cycle
- conditions
    - __limited resources__: not enough to serve all threads simultaneously
    - __no preemption__: cannot force threads to give up resources
    - __cyclical chain of requests__
    - __hold and wait__: threads hold resources while waiting to acquire other resources
- to prevent:
    - eliminating __cyclycal chain of requests__
        - impose gloabl ordering of resources and grab resources in order
    - eliminating __hold-and-wait__
        - either grab all resources or grab no resources at all (e.g. `std::scoped_lock`)
        - banker's algorithm
            - phases
                1. __declare__ all resources
                2.  ```
                    while (!done) {
                        acuquire resource if safe
                        work
                    }
                    ```
                3. release all resources
            - grant resources if it's "safe", otherwise block
                - safe means it is guaranteed that all threads can finish in some specific order after the requesting resources are granted

## Monitor

- a monitor = a lock + conditional variables associated with that lock
- to program with monitor:
    - __lock/mutex__ for mutual exclusion
        - allowing critical sections to be executed atomically
    - __condition variable__ for ordering constraints
        - associated with one __mutex__
        - used when thread execution involve __before-after conditions__
            - can't proceed because condition of shared state isn't satisfactory
            - some other thread must do something
            - assign a condition variable for each situation
    - simplest form:
        ```
        lock
        while (!condition) {
            wait
        }

        do stuff

        signal/broadcast about the stuff you did
        unlock
        ```

## References

- Williams, A. (2019). C++ Concurrency in Action, Second Edition. Chapter 1: Hello, world of concurrency in C++. Manning Publications.
- https://wiki.haskell.org/Parallelism_vs._Concurrency
- https://stackoverflow.com/questions/1050222/what-is-the-difference-between-concurrency-and-parallelism
- Winter 2023 EECS 482 slides, Manos Kapritsos