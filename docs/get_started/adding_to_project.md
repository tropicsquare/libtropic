# Adding Libtropic to an Existing Project
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
# You can use trezor_crypto as the cryptography provider.
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to the libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Linking
target_link_options(produced_binary PRIVATE <your_linker_flags>)

```


!!! note
    The exact CMake calls depend on a configuration of the project into which libtropic is being added. For more inspiration, refer to the [Platform Repositories](../index.md#platform-repositories) section.

!!! note
    We offer multiple CMake options - to see all of them, go to the beginning of the `CMakeLists.txt` file in the repository's root directory.


## Do You Use Makefile Instead of CMake?
In this case, you have to list all libtropic `*.c` and `*.h` files manually inside your Makefile and then for every CMake option you need (located in the libtropic's root `CMakelists.txt`), you add the `-D` switch when building with Make. The same has to be done for the cryptographic provider library, for example in `vendor/trezor_crypto/`.