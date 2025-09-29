# Compiling as a Static Library
Apart from building the libtropic during your project's build process, you can build the libtropic separately as a static library (also known as static archive on Linux) and link it later.

## Compilation
When compiling libtropic as a static library, a cryptography provider must always be defined, either in your `CMakeLists.txt` file or via the CMake command-line arguments.

To compile `libtropic` as a static library on a Unix-like system, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_CRYPTO=trezor_crypto .. # providing the crypto backend
$ make
```

To cross-compile `libtropic` as a static library on a Unix-like system, you need a toolchain configuration file and a linker script. Both should be provided by the vendor of your platform. You can see an example of the toolchain configuration and the linker script in the [libtropic-stm32 repository](https://github.com/tropicsquare/libtropic-stm32).

After acquiring both the toolchain configuration and the linker script, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_CRYPTO=trezor_crypto -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
$ make
```

## Linking in a CMake project
To link the compiled static library to your application, use [`target_link_libraries`](https://cmake.org/cmake/help/latest/command/target_link_libraries.html). For example:

```cmake
add_executable(my_app source1.c source2.c etc.c)
target_link_libraries(my_app <absolute path to library file>)
```

> [!NOTE]
> There are other options for linking the library (e.g., imported targets). Refer to the CMake documentation for more information.

## Linking in a Make Project
If you are using `make` with a Makefile, you can include the static library by adding the following lines to your Makefile:

```makefile
LDFLAGS += -L<directory where the static library file is located>
LDLIBS  += -ltropic
```

> [!NOTE]
> Refer to the [GNU Make documentation](https://www.gnu.org/software/make/manual/html_node/index.html) for more information about linking external libraries.