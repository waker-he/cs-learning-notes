# Content

- [General](#general)
    - [Endianness](#endianness)
    - [Data Alignment](#data-alignment)
    - [Overheads of Function Call](#overhead-of-function-calls)
- [Assembly](#assembly)
    - [ISA](#isa-instruction-set-architecture)
    - [Running Assembly](#running-assembly)
- [Cache](#cache)
    - [Cache Organization](#cache-organization)
    - [Cache Design](#cache-design)

# General

## Endianness

- defines order of integral __bytes__ stored in memory
- Little Endian vs Big Endian
    - Little Endian: higher the address, more significant the byte
        - used more commonly in desktop and server computers
        - x86, Intel, AMD
    - Big Endian: used by many network protocols

## Data Alignment

- the address of data should be a multiple of the size of data
    - for struct (composite data)
        - address must be the multiple of size of its largest primitive-type members
        - total size is multiple of its largest primitive-type members
- benefits
    - access efficiency: modern CPUs access memory in chunks that are the size of a word, if not aligned, may need to access two words for one single unit of data
    - cache optimizations: avoid unnecessary extra cache lines loading

## Overheads of Function Call

- instruction cache
    - jumps to a different area of memory
    - causes instruction cache misses
- extra instructions needed
    - saving and restoring of registers
        - __callee-saved registers__: callee push registers onto stack at the beginning and pop them at the end
        - __caller-saved registers__: saved before a function call and restore after the call
    - parameter passing: push on stack or mov in registers
    - push and pop return address
    - manipulating frame pointer
- possible pipeline stalls
    - instead of just PC=PC+1, need to wait for other instructions like `jump` or `pop eip` to get the next instruction to execute
- indirection
    - function call made through function pointers and virtual methods needs to be resolved at runtime
    - cannot be `inline`d

# Assembly

## ISA (Instruction Set Architecture)
specification for a processor
- how many registers and size of each register
- size of (main) memory
- addressability
    - defines the fundamental unit of memory that needs a unique address to access
    - byte-addressable: every byte has a unique address
    - word-addressable: every word has a unique address
        - word is a natural unit of data processed by a processor
            - _typically_ same size as register and memory address
- instructions that can be use

## Running Assembly

1. Assembling: convert assembly code into object files
2. Linking: link object files into an executable
    - enables separate compilation, unchanged files need not be recompiled
    - adjusts relative offset when combining object files
    - resolves declared but undefined global entities (variables and functions)
3. Loading: place an executable into memory, asks the os to shedule it as a new process


# Cache

- Commonly refers to SRAM used on-chip within the processor
    - even DRAM (main memory) is a "cache" in the sense that it temporarily stores data fetched from disk
- Main reason for using cache: __Temporal locality__
    - memory location accessed recently is more likely to be accessed later than any other random locations
    - need faster access time for these memory locations

## Cache Organization

A cache memory consists of multiple cache lines. A cache line consists of:
- metadata
    - tag
        - in content-addressable memory (CAM)
        - can be searched in parallel
    - dirty bit
        - used for write-back policy
    - bits for replacement policy
        - LRU (`n * logn`), LRU-like, LFU, FIFO
- data block (SRAM)

## Cache Design

### Goal

- reduce AMAT (average memory access time)
    - increase hit rate
    - reduce hit access time
        - reduce tag search time (can be too much even search in parallel)
- reduce size of overheads (metadata)

### Size of Data Per Cache Line

- __Naive way__: keep one byte of data per cache line
- keep more than one byte of data per cache line
- benefits over __naive way__:
    - increase hit rate
        - __spactial locality__: when a memory location is accessed, its nearby locations are more likely to be accessed than any other random locations
    - reduce size of overheads
        - reduce the number of cache lines, which reduces number of metadata needed
        - reduce tag bits

### Associativity

- partitions memory into multiple regions, associativity describes which set of cache lines each memory region can go to.
    - __Naive way__: fully-associative
        - only 1 set, each memory block can go to any cache line
    - direct-mapped
        - `#Sets = #CacheLines`, each memory block can only go to a specific cache line
    - set-associative
        - n-way associative: a set contains n cache lines and `#sets = #CacheLines / n`
        - `set_index_size = log2(#sets)`
- benefits over __naive way__
    - reduce search time with smaller tag bits
        - `tag_size = addr_size - set_index_size - block_offset_size`
    - reduce size of overheads
        - reduces tag size
        - reduces LRU bits size if LRU replacement policy is used

### Cache Size

- bigger can exploit temporal locality better (with lower miss rate)
- too large will increse access time
