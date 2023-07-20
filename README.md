# TestOS (working name)

TestOS is project focused on creating 64-bit monolithic kernel for x86 architecture.

# Technologies used

 - C 11 (with plans to move to C 23 due to new features like #embed)
 - x86 assembly
 - [limine boot protocol](https://github.com/limine-bootloader/limine)

# Goals

TestOS aims to create kernel with following features:

 - Multi-tasking
 - Multi-user
 - Scalability
 - Support for modern devices 

# Build instructions

## Prerequisites

 - GCC compiler (>=9.4.0)
 - Linker (such as lld >=10.0.0)
 - NASM (>=2.14.02)
 - xorriso (>=1.5.2)
 - limine deploy tool (included as git submodule - v3.17.1)

## Build
Makefile has two targets: `setup` and `build-kernel`.
`make setup` will create necessary folder structure for building.
`make build-kernel` will create `TestOS.iso` file in `dist` folder, which is ready to load into VM.
