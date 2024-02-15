> concurrency is a property of a program or system (and parallelism as the run-time behaviour of executing multiple tasks at the same time) <br> – 
Adrian Mouat

# Parallelism

- The term Parallelism refers to techniques to make programs faster by performing several computations at the same time. This requires hardware with multiple processing units.
- Graphic computations on a GPU are parallelism.
- A key problem of parallelism is to reduce data dependencies in order to be able to perform computations on independent computation units with minimal communication between them. To this end, it can even be an advantage to do the same computation twice on different units.

# Concurrency

- A condition that exists when at least two threads are making progress, they can run at the same time but not necessarily.
- Multiple processing units in hardware is not needed, multi-tasking operating system can create an illusion that these threads are running at the same time by time-slicing on a single processing unit.

# Reference

- https://wiki.haskell.org/Parallelism_vs._Concurrency
- https://stackoverflow.com/questions/1050222/what-is-the-difference-between-concurrency-and-parallelism