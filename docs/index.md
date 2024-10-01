# Introduction

`libtropic` is a C library, which implements functionalities for interfacing applications with TROPIC01 device. It comes without any security claims and shall be used for evaluation purpose only.


|Supported devices                                       |Description                                              |
|--------------------------------------------------------|---------------------------------------------------------|
|[TROPIC01](https://www.tropicsquare.com/TROPIC01)       | First generation TROPIC01                               |

# Repository overview

Codebase consists of:
* `examples`: C code concepts to show how libtropic might be used on host system
* `hal`: Contains `crypto` and `port` interfaces. Files in these folders allows libtropic to be used with various cryptography libraries and on different hardware platforms. Both `crypto` and `port` folders are meant to be compiled within a main project (the one which consumes libtropic library). They provide a way for libtropic core to interact with host's cryptography primitives and hardware layer.
* `src`: Libtropic's core source files, facilitating data handling between host's application and TROPIC01 chip
* `tests`: Unit and integration tests
* `vendor`: External libraries and tools

# Library usage

## Dependencies

Used build system is **cmake 3.21**:

```
$ sudo apt install cmake
```

## Options

This library was designed to be compiled during the build of a parent project.

It provides following options to be defined during building:

```
option(USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(LT_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider"       OFF)
option(BUILD_DOCS        "Build documentation"                          OFF)
```

Options could be passed as a command line argument, or they could be defined in main project's cmake files when this library is added to its build tree.


## Examples

List of projects which uses `libtropic`:
* `tests/integration/integration_tests.md`: Unix program to test API calls against model of TROPIC01
* `STM32 example`: Firmware for STM32f429 discovery board which implements libropic's API calls against TROPIC01 chip (or model)

## Static archive

*Note: When compiling the library standalone as a static archive, a cryptography provider must be defined through cmake -D arguments*

Compile `libtropic` as a static archive under Unix:

```
$ mkdir build
$ cd build
$ cmake -DUSE_TREZOR_CRYPTO=1 ..
$ make
```

Cross-compile `libtropic` as a static archive:

```
$ mkdir build
$ cd build
$ cmake -DUSE_TREZOR_CRYPTO=1 -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
$ make
```

## Build documentation

We use Doxygen (1.9.1) and LaTeX (pdfTeX 3.141592653-2.6-1.40.24 (TeX Live 2022))

Build html docs:
```
$ mkdir build/
$ cd build/
$ cmake -DBUILD_DOCS=1 ..
$ make doc_doxygen
```

Build pdf documentation:
```
$ cd docs/doxygen/latex
$ make
```

# Testing

## Unit tests

Unit tests files are in `tests/unit/` folder. They are written in [Ceedling](https://www.throwtheswitch.com) framework, install it like this:

```
    # Install Ruby
    $ sudo apt-get install ruby-full

    # Ceedling install
    $ gem install ceedling

    # Code coverage tool used by Ceedling
    $ pip install gcovr
```

Then make sure that you have correct version:

```
$ ceedling version
   Ceedling:: 0.31.1
      Unity:: 2.5.4
      CMock:: 2.5.4
 CException:: 1.3.3

```

Once ceedling is installed, run tests and create code coverage report:

```
$ ceedling gcov:all utils:gcov
```

### Randomization
Some tests use a rudimentary randomization mechanism using standard C functions. The PRNG is normally
seeded with current time. Used seed is always printed to stdout. You can find the seed in logs
(`build/gcov/results/...`).

To run tests with fixed seed, set `RNG_SEED` parameter to your
desired seed (either directly in the file, or in the project.yml section `:defines:`). This is
useful mainly to replicate a failed test run -- just find out what seed was used and then
set `RNG_SEED` to this.

## Integration tests

For more info check `tests/integration/integration_tests.md`
