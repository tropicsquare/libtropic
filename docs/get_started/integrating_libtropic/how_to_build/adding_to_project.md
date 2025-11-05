# Add to an Existing Project
We recommend adding Libtropic to an existing project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Libtropic uses the CMake build system, so it can be added to the compilation of existing CMake projects as follows:

1. Set path to the Libtropic submodule, for example as:
```cmake
set(PATH_LIBTROPIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/libtropic/)
```
2. Add the Libtropic subdirectory:
```cmake
add_subdirectory(${PATH_LIBTROPIC} "libtropic")
```
3. By default, Libtropic does not link a CFP (Cryptographic Functionality Provider) or its CAL (Crypto Abstraction Layer), so it can be built as a static library. This is the consumer's responsibility:
    1. For the chosen CFP (e.g. MbedTLS v4.0.0), add the correct subdirectory inside `libtropic/cal/`, which provides the corresponding CAL sources and include directories:
    ```cmake
    add_subdirectory("${PATH_LIBTROPIC}cal/mbedtls_v4")
    ```
    2. Add the obtained sources and include directories to the `tropic` target:
    ```cmake
    target_sources(tropic PRIVATE ${LT_CAL_SRCS})
    target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})
    ```
    3. Link the CFP (provided by the consumer) to the `tropic` target:
    ```cmake
    target_link_libraries(tropic PUBLIC mbedtls)
    ```
4. By default, libtropic does not link platform-specific code or its HAL, so it can be built as a static library. This is the consumer's responsibility:
    1. For the chosen platform (e.g. Linux with HW SPI), add the correct subdirectory inside `libtropic/hal/`, which provides the corresponding HAL sources and include directories:
    ```cmake
    add_subdirectory("${PATH_TO_LIBTROPIC}hal/linux/spi")
    ```
    2. Add the obtained sources and include directories to the `tropic` target:
    ```cmake
    target_sources(tropic PRIVATE ${LT_HAL_SRCS})
    target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})
    ```
5. And finally, link Libtropic with your binary:
```cmake
target_link_libraries(my_binary_name PRIVATE tropic)
```

!!! note
    The exact CMake calls depend on a configuration of the project into which libtropic is being added. For more inspiration, refer to the [Integration Examples](../integration_examples.md) section and the [CMake Documentation](https://cmake.org/cmake/help/latest/index.html).

!!! info
    We offer multiple CMake options - to see all of them, go to the beginning of the `CMakeLists.txt` file in the repository's root directory.


## Do You Use a Makefile Instead of CMake?
If you use a Makefile instead of CMake, you need to:

1. Manually list all `*.c` and `*.h` Libtropic files in your Makefile (you can use the root `CMakeLists.txt` for inspiration).
2. For each required CMake option `<CMAKE_OPTION>`, add the `-D<CMAKE_OPTION>` flag when building with Make.

!!! info
    You can find available CMake options in the root `CMakeLists.txt` of the libtropic.

!!! tip
    You can compile libtropic as a static library (see [Compile as a Static Library](compile_as_static_library.md)) using CMake separately and include only the resulting library file in your Makefile.
    This approach eliminates the need to compile the entire libtropic library and its dependencies in your Makefile. However, you will still need to manually add the HAL files for your platform (`libtropic/hal/`) and the CAL files for your CFP (`libtropic/cal/`).