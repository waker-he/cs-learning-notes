# Content

- [General](#general)
- [Assembly](#assembly)
    - [ISA](#isa-instruction-set-architecture)
    - [Running Assembly](#running-assembly)

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

## Overhead of Function Calls

- saving and restoring of registers
    - __callee-saved registers__: callee push registers onto stack at the beginning and pop them at the end
    - __caller-saved registers__: saved before a function call and restore after the call
- instruction cache
    - jumps to a different area of memory
    - causes instruction cache misses
- extra instructions needed
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
