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
3. Loading: place an executable into memory, allowing the processor to run the program

