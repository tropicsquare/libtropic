# Debugging
When debugging, some additional compiler flags are needed to produce debugging information. These flags can be enabled using CMake's [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) setting by:

1. specifying `-DCMAKE_BUILD_TYPE=Debug` when running `cmake` from the command line,
2. adding `set(CMAKE_BUILD_TYPE "Debug")` to your or the libtropic top-level `CMakeLists.txt`.

After this, you can use debugging tools of your choice, e.g. [gdb](https://www.gnu.org/savannah-checkouts/gnu/gdb/index.html) or [Valgrind](https://valgrind.org/). Refer to each tool's documentation for installation instructions.

If you want to use [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer), you need to add additional compiler flags. We provide the `LT_ASAN` option in the top-level `CMakeLists.txt`, along with the `LT_ASAN_COMPILE_FLAGS` and `LT_ASAN_LINK_FLAGS` variables. If your project is using CMake, you can add the following lines to your `CMakeLists.txt` file:
```cmake
if(LT_ASAN)
    message(STATUS "AddressSanitizer (ASan) is enabled for the entire project.")
    add_compile_options(${LT_ASAN_COMPILE_FLAGS})
    add_link_options(${LT_ASAN_LINK_FLAGS})
endif()
```
After that, you will be able to link the static version of [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer), if `LT_ASAN` is set either via the command line or directly in `CMakeLists.txt`.

!!! tip "Tip: Debugging with the TROPIC01 Model"
    If you are debugging on any Unix-like operating system, you do not have to make any changes and can directly debug with the [TROPIC01 Model](../other/tropic01_model/index.md), where the environment for debugging is already prepared (the section provides all information on how to do that).