## [Index](../../README.md)

# Thread Pool

On most systems, it’s impractical to have a separate thread for every task that can potentially be done in parallel with other tasks, but you’d still like to take advantage of the available concurrency where possible. A thread pool allows you to accomplish this; tasks that can be executed concurrently are submitted to the pool, which puts them on a queue of pending work. Each task is then taken from the queue by one of the worker threads, which executes the task before looping back to take another from the queue.

- example thread pool with a fixed number of threads: [`thread_pool.cpp`](./thread_pool.cpp)