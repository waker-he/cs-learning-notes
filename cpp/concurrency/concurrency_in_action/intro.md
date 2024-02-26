# [Contents](./concurrency_in_action.md)

# Chapter 1: Hello, world of concurrency in C++!

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
    - starting a process consumes more resources and overhead then thread
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

# Other References

1. https://wiki.haskell.org/Parallelism_vs._Concurrency
2. https://stackoverflow.com/questions/1050222/what-is-the-difference-between-concurrency-and-parallelism