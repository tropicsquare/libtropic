# Integration Examples
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