## [Index](../README.md)

# Design Concurrent Code

## Techniques for dividing work between threads

- __dividing data between threads before processing begins__: a task is split into a set of parallel tasks, the worker threads run these tasks independently, and the results are combined in a final reduction step. For a simple `for_each`, the final step is a no-op because there are no results to reduce.
- __dividing data recursively__: sometimes it’s only by processing the items that you know how to divide the data. If you’re going to parallelize this algorithm, you need to make use of the recursive nature.
- __dividing work by task type__: An alternative to dividing the work is to make the threads specialists, where each performs a distinct task. Threads may or may not work on the same data, but if they do, it’s for different purposes.
    - __separate concerns__: This is the sort of division of work that results from separating concerns with concurrency; each thread has a different task, which it carries out independently of other threads. Occasionally other threads may give it data or trigger events that it needs to handle, but in general each thread focuses on doing one thing well. In itself, this is basic good design; each piece of code should have a single responsibility.
        - __caveat__: if there is a lot of data shared between the threads or the different threads end up waiting for each other; both cases boil down to too much communication between threads. If this happens, it’s worth looking at the reasons for the communication. If all the communication relates to the same issue, maybe that should be the key responsibility of a single thread and extracted from all the threads that refer to it.
    - __dividing a sequence of tasks between threads__: If your task consists of applying the same sequence of operations to many independent data items, you can use a ___pipeline___ to exploit the available concurrency of your system. To divide the work this way, you create a separate thread for each stage in the pipeline—one thread for each of the operations in the sequence. When the operation is completed, the data element is put in a queue to be picked up by the next thread.
        - __appropriate when data is dynamically generated__
        - __change performance profile__:
            - __generalist threads approach:__ finish process a batch of items every x seconds
            - __specialist threads approach:__ finish processing an item every y seconds (x is y * numOfThreads)
                - have a slow start and the overall time to process the entire batch takes longer as you have to wait until the pipeline is ___primed___ (when the final thread starts processing the first item). But after primed, you get smoother, more regular processing and it can be beneficial in some circumstances like video streaming (viewers are probably happy to accept a delay of a couple of seconds when they start watching a video).

## Factors affecting the performance of concurrent code

- __number of processors (or hardware threads if processor has multiple cores)__
- __oversubscription and excessive task switching__:
    - this can compound any cache problems resulting from lack of proximity
    - if multiple threads call a function that uses `std::thread::hardware_concurrency()` for scaling at the same time, there can still be oversubscription
        - `std::async()` avoids this problem because the library is aware of all calls and can schedule appropriately.
        - Careful use of thread pools can also avoid this problem.
- __Data contention and cache ping-pong__
    - If two threads are executing concurrently on different processors and they’re both reading the same data, this usually won’t cause a problem; the data will be copied into their respective caches, and both processors can proceed. But if one of the threads modifies the data, this change then has to propagate to the cache on the other core, which takes time. In terms of CPU instructions, this can be a _phenomenally_ slow operation, equivalent to many hundreds of individual instructions
    - With high data contention where there are many processors invovled, the processors might find themselves waiting for each other, the data will be passed back and forth between the caches many times, this is called ___cahce ping-pong___
    - The effects of contention with mutexes are usually different from the effects of contention with atomic operations for the simple reason that the use of a mutex naturally serializes threads at the operating system level rather than at the processor level. If you have enough threads ready to run, the operating system can schedule another thread to run while one thread is waiting for the mutex, whereas a processor stall prevents any threads from running on that processor. But it will still impact the performance of those threads that are competing for the mutex; they can only run one at a time, after all.
    - example: __cache ping-pong__ effects can nullify the benefits of reader/writer mutex since all threads accessing the data still have to modify the mutex itself
- __False sharing__: a situation when cache line is shared between multiple threads but none of the data within the cahce line is.
    - use `alignas(std::hardware_destructive_interference_size)` or padding to put potentially unrelated data on different cache lines
- __Data proximity__:
    - __Important for both single-threaded and multi-threaded code__: If there are more threads than cores in the system, each core is going to be running multiple threads. This increases the pressure on the cache, as you try to ensure that different threads are accessing different cache lines in order to avoid false sharing. Consequently, when the processor switches threads, it’s more likely to have to reload the cache lines if each thread uses data spread across multiple cache lines than if each thread’s data is close together in the same cache line.
    - __Solution__: structure the data so that data items to be accessed by the same thread are close together in memory (and thus more likely to be in the same cache line), whereas those that are to be accessed by separate threads are far apart in memory and thus more likely to be in separate cache lines.

## Scalability and Amdahl's law

- Scalability is all about ensuring that your application can take advantage of additional processors in the system it’s running on. At one extreme you have a single-threaded application that’s completely unscalable.
- Amdahl's law states that the overall speedup of a program is limited by the portion of the program that cannot be parallelized.
    - The basic formula is: `Speedup = 1 / (S + P/N)`
        - S = Fraction of execution time spent on serial parts
        - P = Fraction of execution time spent on parts that can be parallelized
        - N = Number of processors
    - Key implications:
        - There's a limit to how much speedup can be achieved through parallelization.
        - The serial portion of a program ultimately becomes the bottleneck.
        - To achieve significant speedups, it's crucial to minimize the serial portion of a program.
