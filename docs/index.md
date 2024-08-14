# libtropic

## Introduction

This library implements functionalities for interfacing applications with TROPIC01 device. It comes without any security claims and shall be used for evaluation purpose only.

Supported TROPIC devices:

|Device                                                  |Comment                                               |
|--------------------------------------------------------|------------------------------------------------------|
|[TROPIC01](https://www.tropicsquare.com/TROPIC01)       | First generation TROPIC01                            |

## Common dependencies

Used build system is **cmake 3.21**:

```
$ sudo apt install cmake
```
Specific version of compiler depends on your target platform. 

## Compiling

You can choose compiler by passing a toolchain file to cmake call.
It is also possible to cross compile the library as a static archive:

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake -DLINKER_SCRIPT=<ABSOLUTE PATH>/STM32F429ZITX_FLASH.ld ..
$ make
```

## Debug mode

For compiling in debug mode pass `cmake -DCMAKE_BUILD_TYPE=Debug ..` during cmake call.

## Integration tests and examples

Example of usage can be found in `tests/integration_tests`. For more info check `tests/integration_tests/index.md`


## Unit tests

[Ceedling](https://www.throwtheswitch.com) is used for running tests and creating code coverage report, install it like this:

```
    # Install Ruby
    $ sudo apt-get install ruby-full

    # Ceedling install
    $ gem install ceedling

    # Code coverage tool used by Ceedling
    $ pip install gcovr
```

Then make sure you have Ceedling installed, expected version:

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
## Documentation

We use Doxygen 1.9.1 and LaTeX (TODO VERSION).

Build html docs:
```
$ mkdir build/
$ cd build/
$ cmake -DBUILD_DOCS=1 ..
$ make doc_doxygen
```
Once html docs is built, pdf documentation can be built like this:
```
$ cd docs/doxygen/latex
$ make
```
