# Adding to an Existing Project
We recommend adding libtropic to an existing project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Libtropic uses CMake build system, therefore it could be added to compilation of existing CMake projects in a following way:

```cmake
# Your CMakeLists.txt file

# Set path to the libtropic submodule. For example:
set(PATH_LIBTROPIC ${CMAKE_CURRENT_SOURCE_DIR}/../libtropic/)

# It is necessary to set provider of cryptography functions.
# You can use e.g. Trezor crypto as the cryptography provider.
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to the libtropic's repository root folder.
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Add HAL functions to sources and includes depending on your platform.
# For example, to include Linux SPI HAL to MY_PROJECTS_SOURCES source list:
set(MY_PROJECT_SOURCES
    ${PATH_LIBTROPIC}/hal/port/unix/lt_port_unix_spi.c
)
include_directories(
    ${PATH_LIBTROPIC}/hal/port/unix
)

# Link libtropic with your binary.
target_link_libraries(my_binary_name PRIVATE tropic)
```

!!! note
    The exact CMake calls depend on a configuration of the project into which libtropic is being added. For more inspiration, refer to the [Integration Examples](integration_examples.md) section and the [CMake Documentation](https://cmake.org/cmake/help/latest/index.html).

!!! note
    We offer multiple CMake options - to see all of them, go to the beginning of the `CMakeLists.txt` file in the repository's root directory.


## Do You Use a Makefile Instead of CMake?
If you use a Makefile instead of CMake, you need to:

1. Manually list all `*.c` and `*.h` files from libtropic in your Makefile.
2. For each required CMake option `<CMAKE_OPTION>`, add the `-D<CMAKE_OPTION>` flag when building with Make.

You must also follow the same steps for the cryptographic provider library, such as the one located in `vendor/trezor_crypto/`.

!!! note
    You can find available CMake options in the root `CMakeLists.txt` of the libtropic.

!!! note
    You can compile libtropic as a static library using CMake separately and include only the resulting library file in your Makefile.
    This approach eliminates the need to compile the entire libtropic library and its dependencies in your Makefile. For more details, see [Compiling as a Static Library](./compile_as_static_library.md).