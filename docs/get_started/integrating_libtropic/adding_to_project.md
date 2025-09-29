# Adding to an Existing Project
We recommend adding libtropic to an existing project as a *git submodule*. Libtropic uses CMake build system, therefore it could be added to compilation of existing CMake projects in a following way:

```cmake
# Your CMakeLists.txt file

# Set path of to the libtropic submodule
set(PATH_LIBTROPIC "libtropic/")

# This switch will expose not only core library functions, but also helper functions (recommended)
set(LT_HELPERS ON)

# These switches add functional tests and examples to the build process.
# Might be a good starting point. Comment out if not needed.
set(LT_BUILD_EXAMPLES ON)
set(LT_BUILD_TESTS ON)

# It is necessary to set provider of cryptography functions.
# You can use e.g. trezor_crypto as the cryptography provider.
set(LT_CRYPTO "trezor_crypto")

# Add path to the libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Linking
target_link_options(produced_binary PRIVATE <your_linker_flags>)

```


!!! note
    The exact CMake calls depend on a configuration of the project into which libtropic is being added. For more inspiration, refer to the [Integration Examples](integration_examples.md) section.

!!! note
    We offer multiple CMake options - to see all of them, go to the beginning of the `CMakeLists.txt` file in the repository's root directory.


## Do You Use a Makefile Instead of CMake?
If you use a Makefile instead of CMake, you need to:

1. Manually list all `*.c` and `*.h` files from libtropic in your Makefile.
2. For each CMake option `<CMAKE_OPTION>` you need (found in the root `CMakeLists.txt` of libtropic), add the `-D<CMAKE_OPTION>` flag when building with Make.

You must also follow the same steps for the cryptographic provider library, such as the one located in `vendor/trezor_crypto/`.

!!! note
    You can compile libtropic as a static library using CMake separately and include only the resulting library file in your Makefile.
    This approach eliminates the need to compile the entire libtropic library and its dependencies in your Makefile. For more details, see [Compiling as a Static Library](./compile_as_static_library.md).