# Compiling as a Static Archive
Apart from building the libtropic library during your project's build process, you can build the libtropic library separately as a static archive and link it later.

## Compilation
When compiling libtropic as a static archive, a cryptography provider must always be defined, either in your `CMakeLists.txt` file or via the CMake command-line arguments.

To compile `libtropic` as a static archive on a Unix-like system, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_CRYPTO=trezor_crypto .. # providing the crypto backend
$ make
```

To cross-compile `libtropic` as a static archive on a Unix-like system, you need a toolchain configuration file and a linker script. Both should be provided by the vendor of your platform. You can see an example of the toolchain configuration and the linker script in the [libtropic-stm32 repository](https://github.com/tropicsquare/libtropic-stm32).

After acquiring both the toolchain configuration and the linker script, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_CRYPTO=trezor_crypto -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
$ make
```

## Linking
To link the compiled static archive to your application, use [`target_link_libraries`](https://cmake.org/cmake/help/latest/command/target_link_libraries.html). For example:

```cmake
add_executable(my_app source1.c source2.c etc.c)
target_link_libraries(my_app <absolute_path_to_static_archive>)
```

> [!NOTE]
> There are other options for linking the library (e.g., imported targets). Refer to the CMake documentation for more information.
