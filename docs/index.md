# Libtropic

## Introduction

This library implements functionalities for interfacing applications with TROPIC01 device. It comes without any security claims and shall be used for evaluation purpose only.

Supported TROPIC devices:

|Device                                                  |Comment                                               |
|--------------------------------------------------------|------------------------------------------------------|
|[TROPIC01-sample](https://www.tropicsquare.com/TROPIC01)| Engineering sample, not suitable for production use  |
|[TROPIC0X](https://www.tropicsquare.com/TROPIOC0X)      | Version X TBD                                        |



## Dependencies

Used build system is **cmake 3.21**:

```
    $ sudo apt install cmake
``` 

[Ceedling](https://www.throwtheswitch.com) is used for running tests and creating code coverage report, install it like this:

```
    # Install Ruby
    $ sudo apt-get install ruby-full

    # Ceedling install
    $ gem install ceedling

    # Code coverage tool used by Ceedling
    $ pip install gcovr
```


## Examples

A few examples of library's usage are placed in `examples/` folder.


## Running tests

Make sure you have Ceedling installed (as described in Dependencies).

Expected version:

```
$ ceedling version
   Ceedling:: 0.31.1
      Unity:: 2.5.4
      CMock:: 2.5.4
 CException:: 1.3.3

```

Running tests and creating code coverage report:

```
$ ceedling gcov:all utils:gcov
```

## Library configuration

See option() calls in root **CMakelists.txt** and check also how CMakeLists.txt looks in example projects.

### Cryptography support

For certain operations on application's side, libtropic needs cryptography support. It is possible to choose a cryptography provider, because definitions of crypto functions are chosen during compilation.

Current default provider of cryptogprahy is `vendor/trezor_crypto`.

```
# Use trezor_crypto library:

option(TS_CRYPTO_TREZOR "Use trezor_crypto as a cryptography provider" ON)
```


## Library overview

**Tropic layer 1**

 This layer is processing raw data transfers.

 Available L1 implementations are:

* SPI (Libtropic on embedded target and Physical chip, or FPGA)
* TCP (Libtropic on Unix and Tropic01's model on Unix)
* Serialport (Libtropic on embedded target and Tropic01's model on Unix)

Use `option()` switch to enable libtropic support for a certain platform, have a look in examples.

If there is no support for a platform, user is expected to provide own implementation for weak functions in this layer.

Related code:
* ts_l1.c
* ts_l1.h

**Tropic layer 2**

This layer is responsible for executing l2 request/response functions.

Related code:
* ts_l2.c
* ts_l2.h

**Tropic layer 3**

This layer is preparing and parsing l3 commands/results, it uses l2 functions to send and receive payloads.

Related code:
* ts_l3.c
* ts_l3.h

**Libtropic**

This is a highest abstraction of tropic chip functionalities.
Library offers various calls to simplify tropic chip usage on a target platform:

Related code:
* libtropic.c
* libtropic.h
