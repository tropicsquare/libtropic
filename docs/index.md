# Introduction

Welcome to the documentation page for **libtropic**, the official C library for the [TROPIC01](https://tropicsquare.com/tropic01) secure element.

## Documentation Structure
This documentation has the following structure:

- [Get Started](get_started/index.md): All the things you need to get started with libtropic.
- [Examples](examples/index.md): Dives into our examples and shows basic libtropic usage.
- [API Reference](doxygen/build/html/index.html): Detailed lbtropic API documentation generated with Doxygen.
- [For Contributors](for_contributors/index.md): If you need more information on libtropic to start developing or contributing.
- [Other](other/index.md): Even more information about libtropic.

## TROPIC01

![TROPIC01](img/tropic01_chip.jpeg){ width="200" }

For more information about TROPIC01, refer to the [TROPIC01 repository](https://github.com/tropicsquare/TROPIC01), where you can find datasheets, documentation and ordering instructions.

## Platform Repositories
We offer so-called *platform repositories*, where libtropic is utilized as a submodule, possibly showing an example of integrating libtropic into your application:

- [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32),
- [libtropic-linux](https://github.com/tropicsquare/libtropic-linux).

All of these repositories consist of:

- libtropic as a git submodule,
- directories for supported platforms:
    - `CMakeLists.txt`,
    - include directory,
    - source directory with `main.c`,
    - additional files (readme, scripts, ...).

For more detailed info, refer to the aforementioned platform repositories.

Beside these platform repositories, we offer the [libtropic-util](https://github.com/tropicsquare/libtropic-util) repository, which implements a CLI based utility for executing TROPIC01's commands.