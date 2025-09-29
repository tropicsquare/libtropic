# Adding to an Existing Project
We recommend adding libtropic to an existing project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Libtropic uses CMake build system, therefore it could be added to compilation of existing CMake projects in a following way:

```cmake
# Your CMakeLists.txt file

# Set path to the libtropic submodule. For example:
set(PATH_LIBTROPIC ${CMAKE_CURRENT_SOURCE_DIR}/../libtropic/)

# It is necessary to set provider of cryptography functions.
# You can use e.g. trezor_crypto as the cryptography provider.
set(LT_CRYPTO "trezor_crypto")

# Add path to the libtropic's repository root folder.
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Add HAL functions to sources and includes depending on your platform.
# For example, to include Linux SPI HAL to MY_PROJECTS_SOURCES source list:
set(MY_PROJECT_SOURCES
    ${PATH_LIBTROPIC}/hal/port/unix/libtropic_port_unix_spi.c
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


## Do You Use Makefile Instead of CMake?
In this case, you have to:

1. list all libtropic `*.c` and `*.h` files manually inside your Makefile,
2. for every CMake option `<CMAKE_OPTION>` you need (located in the libtropic's root `CMakelists.txt`), add the `-D<CMAKE_OPTION>` switch when building with Make.

The same has to be done for the cryptographic provider library, for example in `vendor/trezor_crypto/`.