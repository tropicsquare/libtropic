# Add to an Existing Project
We recommend adding Libtropic to an existing project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Libtropic uses CMake build system, therefore it could be added to compilation of existing CMake projects in a following way:

1. Set path to the Libtropic submodule, for example as:
```cmake
set(PATH_LIBTROPIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/libtropic/)
```
2. Add the Libtropic subdirectory:
```cmake
add_subdirectory(${PATH_LIBTROPIC} "libtropic")
```
3. By default, Libtropic does not link crypto backend or it's HAL, so it can be built as a static library. This is the consumer's responsibility:
    1. Choose one of the supported crypto backends (e.g. MbedTLS v4.0.0) and select its HAL:
    ```cmake
    set(LT_CRYPTO_HAL "mbedtls_v4")
    ```
    2. Add the `libtropic/hal/crypto/` subdirectory, which provides the crypto backend HAL sources and include directories:
    ```cmake
    add_subdirectory("${PATH_LIBTROPIC}hal/crypto/")
    ```
    3. Add the provided sources and include directories to the `tropic` target:
    ```cmake
    target_sources(tropic PRIVATE ${LT_CRYPTO_HAL_SRCS})
    target_include_directories(tropic PUBLIC ${LT_CRYPTO_HAL_INC_DIRS})
    ```
    4. Link the cryptographic backend (provided by the consumer) to the `tropic` target:
    ```cmake
    target_link_libraries(tropic PUBLIC mbedtls)
    ```
4. By default, libtropic does not link platform-specific code or it's HAL, so it can be built as a static library. This is the consumer's responsibility:
    1. Add HAL functions to sources and includes depending on your platform. For example, to include Linux SPI HAL to `MY_PROJECTS_SOURCES` source list:
    ```cmake
    set(MY_PROJECT_SOURCES
        ${PATH_LIBTROPIC}/hal/port/unix/libtropic_port_unix_spi.c
    )
    include_directories(
        ${PATH_LIBTROPIC}/hal/port/unix
    )
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
    This approach eliminates the need to compile the entire libtropic library and its dependencies in your Makefile. However, you will still need to manually add the HAL files for your platform (`libtropic/hal/port/`) and the cryptographic backend you want to use (`libtropic/hal/crypto/`).