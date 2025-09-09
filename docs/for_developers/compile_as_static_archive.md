## Compile libtropic as Static Archive

!!! note
    When compiling the library standalone as a static archive, a cryptography provider must be defined through `cmake -D` arguments.

Compile `libtropic` as a static archive under Unix:

```
$ mkdir build
$ cd build
$ cmake -DLT_USE_TREZOR_CRYPTO=1 ..
$ make
```

Cross-compile `libtropic` as a static archive:

```
$ mkdir build
$ cd build
$ cmake -DLT_USE_TREZOR_CRYPTO=1 -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
$ make
```