
# Basic Concurrency API

- [`std::thread`](#stdthread)
- [Protecting Shared Data](#protecting-shared-data)
- [Synchronizing Actions](#synchronizing-actions)
    - [condition variable](#condition-variable)
    - [`std::latch` and `std::barrier` (since C++20)](#stdlatch-and-stdbarrier-since-c20)
    - [Semaphore (since C++20)](#semaphore-since-c20)
    - [Waiting for one-off events with futures](#waiting-for-one-off-events-with-futures)
- [Waiting with a Time Limit](#waiting-with-a-time-limit)
- [Parallel Algorithms](#parallel-algorithms)

## `std::thread`

```cpp
template< class F, class... Args >
explicit thread( F&& f, Args&&... args );
```

- when starting a new thread of execution, passed function objects and arguments are copied/moved into the storage belonging to the new thread
- if passing rvalue argument to `std::thread` ctor, then it will be moved, otherwise it is copied
- if function has pass-by-reference parameters, then the reference will refer to the copy of the object in the storage of new thread instead of the original object that is in the storage of the parent thread
    - to bypass this copy, use `std::ref`, then it is `std::ref` that is being copied/moved instead of the referenced object

```cpp
~thread();
```

- if `std::thread` is joinable, `std::terminate()` is called
- prefer [C++20 `std::jthread`](../c++20/cpp20.md#chapter-12-stdjthread-and-stop-tokens) than `std::thread`

## Protecting Shared Data

- `std::mutex`: most basic synchronization primitive providing mutual exclusion for threads to access shared data exclusively
- `std::shared_mutex`: reader-writer mutex, allows multiple reader threads (through `lock_shared`) or exclusive access (through `lock`)
    - useful for data-structure that is rarely updated
- `std::recursive_mutex`: allows acquiring several locks on a single instance from the __same thread__, number of `unlock()` must match number of `lock()` so that other thread can lock this mutex
    - use it cautiously since it may be a sign of bad design

### RAII Types for Mutex

- `std::lock_guard<MutexType>`
    - RAII type for handling `mutex`
    - held mutex cannot be `unlocked` until destruction
    - can be considered _deprecated_ since C++17 `std::scoped_lock`
- `std::scoped_lock<MutexTypes...>`:
    - owns any number of mutexes
    - held mutexes cannot be `unlocked` until destruction
- `std::unique_lock<MutexType>`: more flexible locking
    - `std::movable`
    - mutex can be owned or not owned during its lifetime
    - overheads that come with flexible locking
        - a flag variable to keep track of whether the lock owns its mutex
        - `if` branches in dtor
- `std::shared_lock<std::shared_mutex>`
    - lock the mutex in shared mode `mutex.lock_shared()`
    - same flexibility as `std::unique_lock`

## Synchronizing Actions

### condition variable
- `std::condition_variable`
    - thread execution involve __before-after conditions__
        - can't proceed because condition of shared state isn't satisfactory
        - some other thread must grab the lock and do something on the shared state
        - assign a condition variable for each situation
    - `wait` only accepts `std::unique_lock<std::mutex>`
    ```cpp
    std::mutex mut;
    std::queue<int> data_queue;
    std::condition_variable data_cond;
    void producer() {
        int const data = prepare_data();
        {
            std::scoped_lock lk(mut);   // CTAD
            data_queue.push(data);
        }
        data_cond.notify_one();
    }

    void consumer() {
        while (true) {
            std::unique_lock lk(mut);
            // use while loop to handle spurious wake
            while (!data_queue.empty())
                data_cond.wait(lk);
            // equivalent to:
            // data_cond.wait(lk, []{ return !data_queue.empty(); });

            int data = data_queue.front();
            data_queue.pop();
            lk.unlock();    // avoid processing data while holding lock
            process(data);
            if (is_last(data)) break;
        }
    }
    ```
- `std::condition_variable_any`: allows argument of `wait` to be any lockable types
    - more expensive than `std::condition_variable`
        - runtime polymorphism to lock and unlock the object
        - heap allocation to store lockable object
        - `sizeof(std::condition_variable_any) > sizeof(std::condition_variable)`
- https://en.cppreference.com/w/cpp/thread/condition_variable

### `std::latch` and `std::barrier` (since C++20)

- more specific than condition variables, the condition waiting for is a counter to be counted down to 0
- __latch__: supports single-use asynchronous countdown
    - can be count down by the same thread more than once
    - https://en.cppreference.com/w/cpp/thread/latch
- __barrier__: supports multiple-use asynchronous countdown and allows register callback to be called when reaching zero
    - one thread can only count down once
    - https://en.cppreference.com/w/cpp/thread/barrier

### Semaphore (since C++20)

- light-weight synchronization primitive that allow you to synchronize or restrict access to one or a group of resources
    - can be used both for protecting shared data and synchronizing actions
    - `acquire` decrements the internal counter or blocks until it can
    - `release` increments the internal counter and unblocks acquirers
- `std::counting_semaphore<>` limits the use of multiple resources up to a maximum value
- `std::binary_semaphore` limits the use of a single resource
    - `std::counting_semaphore<1>`
- https://en.cppreference.com/w/cpp/thread/counting_semaphore

### Waiting for one-off events with futures

- when using __futures__, the condition waiting for is a one-off event
    - more specific than condition variables, which can be used for waiting repeating event
    - the event is usually some data to process, but there can also be no data involved (`std::future<void>`)
    - once the future becomes ready, it cannot be reset
- `std::future` and `std::shared_future` are modeled after `std::unique_ptr` and `std::shared_ptr`
    - an instance of `std::future` is the one and only instance that refers to its associated event
    - whereas multiple instances of `std::shared_future` can refer to the same event
    - `std::future` can transfer shared state to a `std::shared_future` through move or call to `share()`
- future objects themselves don’t provide synchronized accesses, multiple threads may each access their own copy of `std::shared_future<>` without further synchronization
- future provides a mechanism to access the result of asynchronous operations
    - An asynchronous operation (created via `std::async`, `std::packaged_task`, or `std::promise`) can provide a `std::future` object to the creator of that asynchronous operation.
    - `std::async`: start an _asynchronous task_ and returns a `std::future` object, which will eventually hold the return value of the function
    - `std::packaged_task<>`: a wrapper that ties a future to a callable
        - When invoked, it calls the associated function or callable object and makes the future ready, with the return value stored as the associated data
        - If a large operation can be divided into self-contained sub-tasks, each of these can be wrapped in a `std::packaged_ task<>` instance, and then that instance passed to the task scheduler or thread pool
        ```cpp
        template<>
        class packaged_task<std::string(std::vector<char>*,int)> {
        public:
            template<typename Callable>
            explicit packaged_task(Callable&& f);
            std::future<std::string> get_future();
            void operator()(std::vector<char>*,int);
        };
        ```
    - `std::promise`: used when the asynchronous operation cannot be expressed as a simple function call
        - You can obtain the `std::future` object associated with a given `std::promise` by calling the `get_future()` member function, just like with `std::packaged_task`.
        - When the value of the promise is set (using the `set_value()` member function), the future becomes ready and can be used to retrieve the stored value
- saving the __exception__ for the future
    - if the function call invoked as part of `std::async` or `std::packaged_task` throws an exception, that exception is stored in the future in place of a stored value, the future becomes ready, and a call to `get()` rethrows that stored exception
    - `std::promise` can use `set_exception()` member function:
        ```cpp
        extern std::promise<double> some_promise;
        try { some_promise.set_value(calculate_value()); }
        catch (...) { some_promise.set_exception(std::current_exception); }
        ```
    - Another way to store an exception in a future is to destroy the `std::promise` or `std::packaged_task` associated with the future without calling either of the set functions on the promise or invoking the packaged task
        - by creating a future you make a promise to provide a value or exception, and by destroying the source of that value or exception without providing one, you break that promise.
        - `std::future_errc::broken_promise` will be stored in the shared state
- use `std::experimental::when_all` and `std::experimental::when_any` to wait for all or any one of a group of futures to be ready
- `std::experimental::future` has member function `then()` to execute a continuation function when the future becomes ready
- https://en.cppreference.com/w/cpp/thread/future
- https://en.cppreference.com/w/cpp/experimental/concurrency

## Waiting with a Time Limit

- uses [`<chrono>`](../cppcon/chrono/chrono.md)
- There are two sorts of timeouts you may wish to specify:
    - a duration-based timeout, where you wait for a specific amount of time (for example, 30 milliseconds);
    - or an absolute timeout, where you wait until a specific point in time (for example, 17:30:15.045987023 UTC on November 30, 2011).
    - Most of the waiting functions provide variants that handle both forms of timeouts. The variants that handle the duration-based timeouts have a `_for` suffix, and those that handle the absolute timeouts have an `_until` suffix.

## Parallel Algorithms (since C++17)

- The C++17 standard added the concept of parallel algorithms to the C++ Standard Library. These are additional overloads of many of the functions that operate on ranges, such as `std::find`, `std::transform` and `std::reduce`. The parallel versions have the same signature as the “normal” single-threaded versions, except for the addition of a new first parameter, which specifies the ___execution policy___ to use
    ```cpp
    std::vector<int> my_data;
    std::sort(std::execution::par, my_data.begin(), my_data.end());
    ```
- the execution policy is _permission_, not a _requirement_--the library may still execute the code on a single thread if it wishes
- if the “normal” algorithm allows ___Input Iterators___ or ___Output Iterators___, then the overloads with an execution policy require ___Forward Iterators___ instead, the requirement that incrementing a ___Forward Iterator___ does not invalidate other copies is important, as it means that separate threads can operate on their own copies of the iterators, incrementing them when required, without concern about invalidating the iterators held by the other threads.

### general effects of specifying an execution policy

- __the algorithm's complexity__
    - many parallel algorithms will perform more of the core operations of the algorithm (whether swaps, comparisons, or applications of a supplied function object) to take advantage of parallelism
- __the behavior when an exception is thrown__
    - uncaught exception during execution will cause `std::terminate` instead of propagating to caller
        - this is the main diffeence between using `std::execution::seq` and not providing an execution policy
    - The only exception that may be thrown by a call to a standard library algorithm with one of the standard execution policies is std::bad_alloc, which is thrown if the library cannot obtain sufficient memory resources for its internal operations.
- __where, how, and when the steps of algorithm are executed__
    - This is the fundamental aspect of an execution policy, and is the only aspect that differs between the standard execution policies

### policies

- `std::execution::seq`: This policy specifies that the algorithm will be executed sequentially and there will be no parallelism.
    - the operation order is unspecified and may be different between different invocations, for example, `std::find` might not return the first element matched
- `std::execution::par`: This policy specifies that the algorithm may be parallelized which can take advantage of multiple cores in your processor for faster execution.
    - passed callable objects cannot have data race, synchronization is permitted
- `std::execution::par_unseq`: This policy specifies that the algorithm may be parallelized and vectorized. Vectorization is a form of parallelism where the same operation is applied to different data in parallel, which can further improve performance on modern processors that support SIMD (Single Instruction Multiple Data) instructions.
    - any form of synchronization is not allowed



## References

- Williams, A. (2019). C++ Concurrency in Action, Second Edition. Chapter 2: Managing threads. Manning Publications.
- Williams, A. (2019). C++ Concurrency in Action, Second Edition. Chapter 3: Sharing data between threads. Manning Publications.
- Williams, A. (2019). C++ Concurrency in Action, Second Edition. Chapter 4: Synchronizing concurrent operations. Manning Publications.
- Williams, A. (2019). C++ Concurrency in Action, Second Edition. Chapter 10: Parallel algorithms. Manning Publications.
- cppreference