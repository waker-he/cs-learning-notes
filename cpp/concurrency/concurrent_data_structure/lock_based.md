## [Index](./concurrent_data_structure.md)

# Lock-Based Concurrent Data Structures

- __Key of design__: Locking at an appropriate granularity
    - to enable
        - safe concurrent access
        - greater potential for concurrency
            - __serialization__: threads take turns accessing the data protected by the mutex; they must access it serially rather than concurrently.
            - it’s quite common for a data structure to support concurrent access from threads performing different operations while serializing threads that try to perform the same operation.
    - Locking at an appropriate granularity isn’t only about the amount of data locked; it’s also about how long the lock is held and what operations are performed while the lock is held.
- __Caution__:
    - Avoid passing reference to data members to user-supplied code, if user's code stores the reference outside the data structure, the invariant can be easily broken
    - Passing control to user-suppplied code also has risks of deadlock
    - these are more of user's reponsibilities

## Case Study

### threadsafe_queue

- code: [`threadsafe_queue.cpp`](./threadsafe_queue.cpp)
- __Fine-grained Locking__:
    - to enable fine-grained locking, `threadsafe_queue` is not an adapter of `std::queue`
    - there is no deadlock
        - in `push` we only lock `tail_mutex`
        - in `pop`, we always lock `head_mutex` and `tail_mutex` in order
- __Interface Change__
    - `empty()`, `top()` and `pop()` are merged into `try_pop()`/`wait_and_pop()`
    - exceptions might be thrown when returning the popped item if the item's move ctor is not `noexcept`, which can result in lost of the item and violate exception safety, options are:
        1. require the element type to have `noexcept` move ctor (chosen in code example)
        2. pass in a reference and move assign the item to the reference before popping
        3. return pointer to data as copying pointer is `noexcept`
        4. provide overloads to combine any of the above

### thread-safe lookup table

- options:
    - __binary tree__: A binary tree doesn’t provide much scope for extending the opportunities for concurrency; every lookup or modification has to start by accessing the root node, which therefore has to be locked.
    - __sorted array__: A sorted array is even worse, because you can’t tell in advance where in the array a given data value is going to be, so you need a single lock for the whole array.
    - __hash table__: You can safely have a separate lock per bucket.
        - for each bucket, we can get more fine-grained locking by using a thread-safe list which uses a separate mutex for each node and does hand-over-hand locking when traversing
        - use `std::shared_mutex` to get greater potential for concurrency
- __Interface Change__
    - The basic issue with STL-style iterator support is that the iterator must hold some kind of reference into the internal data structure of the container. If the container can be modified from another thread, this reference must somehow remain valid, which requires that the iterator hold a lock on some part of the structure. Given that the lifetime of an STL-style iterator is completely outside the control of the container, this is a bad idea.
    - The alternative is to provide iteration functions such as `for_each` as part of the container itself. This puts the container squarely in charge of the iteration and locking