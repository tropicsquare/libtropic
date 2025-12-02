# Debugging
When debugging, some additional compiler flags are needed to produce debugging information. These flags can be enabled using CMake's [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) option. This option can be set similarly as the Libtropic's CMake options — see [How to Configure](integrating_libtropic/how_to_configure/index.md) section.

After this, you can use debugging tools of your choice, e.g. [gdb](https://www.gnu.org/savannah-checkouts/gnu/gdb/index.html) or [Valgrind](https://valgrind.org/). Refer to each tool's documentation for installation instructions.

If you want to use [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer), you have the following options:

1. You are using the [TROPIC01 Model](../other/tropic01_model/index.md) — refer to its section for more information.
2. You are using one of our [platform repositories](integrating_libtropic/integration_examples.md) — pass `-DLT_ASAN=1` when building with `cmake` on the command line.
3. In other cases, you will have to link it to Libtropic yourself.