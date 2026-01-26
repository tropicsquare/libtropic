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
    1. For the chosen platform (e.g. Linux with HW SPI), add a corresponding HAL using `add_subdirectory`:
    ```cmake
    add_subdirectory("${PATH_TO_LIBTROPIC}hal/linux/spi")
    ```
    2. Add HAL sources and include directories to the `tropic` target. In the previous step, `LT_HAL_SRCS` and `LT_HAL_INC_DIRS` variables were populated based on the selected HAL, so you can use those:
    ```cmake
    target_sources(tropic PRIVATE ${LT_HAL_SRCS})
    target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})
    ```
5. And finally, link Libtropic with your binary:
```cmake
target_link_libraries(my_binary_name PRIVATE tropic)
```

!!! note "Inspiration for CMakeLists.txt"
    The exact CMake calls depend on a configuration of the project into which libtropic is being added. For more inspiration, refer to our standalone example projects in `examples/` (explained in [Tutorials](../../../tutorials/index.md)) and the [CMake Documentation](https://cmake.org/cmake/help/latest/index.html).

!!! info "Supported Host Platforms and CFPs"
    Refer to sections [Supported Host Platforms](../../../compatibility/host_platforms/index.md) and [Supported Cryptographic Functionality Providers](../../../compatibility/cfps/index.md) to see what is supported.


## Do You Use a Makefile Instead of CMake?
If you use a Makefile instead of CMake, you need to:

1. Manually list all `*.c` and `*.h` Libtropic files in your Makefile (you can use the root `CMakeLists.txt` for inspiration).
2. For each required CMake option `<CMAKE_OPTION>`, add the `-D<CMAKE_OPTION>` flag when building with Make.

!!! info "Available CMake Options"
    See [How to Configure](../how_to_configure/index.md) for available CMake Options. However, some of these options are not directly used in the Libtropic code - based on them, additional internal macros are defined. To see those, either:
    
    1. Analyze Libtropic's root `CMakeLists.txt`.
    2. Configure Libtropic using CMake and then execute `grep LT_ CMakeCache.txt` in your `build/` directory to see all used options/defines.

!!! tip "Tip: Build Libtropic as a Static Library"
    You can compile libtropic as a static library (see [Compile as a Static Library](compile_as_static_library.md)) using CMake separately and include only the resulting library file in your Makefile.
    This approach eliminates the need to compile the entire libtropic library and its dependencies in your Makefile. However, you will still need to manually add the HAL files for your platform (`libtropic/hal/`) and the CAL files for your CFP (`libtropic/cal/`).