# Debugging
When debugging, some additional compiler flags are needed to produce debugging information. These flags can be added using the CMake's [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) environment variable by:

1. specifying `-DCMAKE_BUILD_TYPE=Debug` when executing `cmake` in the command line,
2. putting `set(CMAKE_BUILD_TYPE "Debug")` into *yours* or the *libtropic's* main `CMakeLists.txt`.

After this, you can use debugging tools of your choice, e.g. [gdb](https://www.gnu.org/savannah-checkouts/gnu/gdb/index.html) or [Valgrind](https://valgrind.org/). Refer to each tool's documentation for the installation instructions.

However, if you were to use e.g. [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer), you would have to add more compiler flags. That is why we added the `LT_ASAN` option into our main `CMakeLists.txt`, along with the `LT_ASAN_COMPILE_FLAGS` and `LT_ASAN_LINK_FLAGS` variables. If your project is using CMake, you could add the following lines into your `CMakeLists.txt` file:
```cmake
if(LT_ASAN)
    message(STATUS "AddressSanitizer (ASan) is enabled for the entire project.")
    add_compile_options(${LT_ASAN_COMPILE_FLAGS})
    add_link_options(${LT_ASAN_LINK_FLAGS})
endif()
```
and be able to link the static version of [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer), if `LT_ASAN` is set either via the command line or directly in `CMakeLists.txt`. These lines were taken from the project for the [TROPIC01 Model](../other/tropic01_model.md), located in `tropic01_model/`. If you are debugging on any Unix-like operating system, you do not have to make any changes and can directly debug with the [TROPIC01 Model](../other/tropic01_model.md), where the environment for debugging is already prepared (the section provides all information on how to do that).