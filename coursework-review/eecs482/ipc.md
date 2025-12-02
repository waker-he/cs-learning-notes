# IPC

## Signals

- raised by a process and delivered to another process
- destination is the process's signal handler
- many signals are predefined and the process has a default signal handler to deal with it
    - SIGINT (^C): exit
    - SIGKILL (kill -9): cannot customize
    - SIGTERM (kill): customizable
    - SIGUSR1 and SIGUSR2: not predefined
- system call: `int kill(pid_t pid, int sig);`
- customize:
    ```c
    int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
    ```

## Pipe

- a call to the `pipe()` function returns a pair of file descriptors. One of these descriptors is connected to the write end of the pipe, and the other is connected to the read end. Anything can be written to the pipe, and read from the other end in the order it came in. On many systems, pipes will fill up after you write about 10K to them without reading anything out.
- not so useful except for `|` operator:
    - left side of `|`:
        - `close(1)` frees up file descriptor 1 (standard output)
        - `dup(pfds[1])` makes a copy of the write-end of the pipe in the first avialable file descriptor, which is "1"
    - right side of `|`:
        - `close(0)`
        - `dup(pfds[0])`

## FIFOs (named pipe)

- address problem of normal pipe: you can't grab one end of a normal pipe that was created by an unrelated process
- with a name, multiple processes can `open()` and read and write to it
- `mknod()` or `mknod` command
    ```c
    mknod(FIFO_NAME, S_IFIFO | 0666, 0);
    fd = open(FIFO_NAME, O_WRONLY); // will block until other process opens the other end for reading: open(FIFO_NAME, O_RDONLY)
    ```
- two copies when communicating:
    1. writer -> buffer
    2. buffer -> reader
