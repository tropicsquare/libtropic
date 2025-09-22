# Compile Libtropic as a Static Archive
When compiling libtropic as a static archive, a cryptography provider must always be defined, either in your CMakeLists.txt file or via the CMake command line arguments.

To compile `libtropic` as a static archive on an Unix-like system, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_USE_TREZOR_CRYPTO=1 .. # providing the crypto backend
$ make
```

To cross-compile `libtropic` as a static archive on an Unix-like system, do:

```shell
$ mkdir build
$ cd build
$ cmake -DLT_USE_TREZOR_CRYPTO=1 -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
$ make
```