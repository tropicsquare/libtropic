# Hello World!

Welcome to the documentation page for **libtropic**, the official C library for TROPIC01 chip. This guide will help you get started with building applications using **TROPIC01** and **libtropic**.

For detailed information about the libtropic library architecture and development, please refer to the `For contributors` section.

- [Hello World!](#hello-world)
  - [Getting TROPIC01 {#getting-tropic01}](#getting-tropic01-getting-tropic01)
      - [QFN32 Samples](#qfn32-samples)
      - [Breakout PCB with TROPIC01](#breakout-pcb-with-tropic01)
      - [USB dongle with TROPIC01](#usb-dongle-with-tropic01)
  - [Libtropic library examples and tests {#libtropic-library-examples}](#libtropic-library-examples-and-tests-libtropic-library-examples)
  - [Standalone example projects {#standalone-example-projects}](#standalone-example-projects-standalone-example-projects)
      - [STM32](#stm32)
      - [Unix](#unix)
  - [Adding libtropic to existing project {#adding-libtropic-to-existing-project}](#adding-libtropic-to-existing-project-adding-libtropic-to-existing-project)
      - [Do you use Makefile instead of CMake?](#do-you-use-makefile-instead-of-cmake)
  - [Using CMAKE switches {#using-cmake-switches}](#using-cmake-switches-using-cmake-switches)

## Getting TROPIC01 {#getting-tropic01}

`TROPIC01` is currently available in three forms:

- QFN32 4x4mm IC package for direct PCB integration
- Development board with pre-soldered TROPIC01 for easy evaluation
- USB dongle with TROPIC01 for systems without SPI bus

#### QFN32 Samples


<img src="QFN32.jpg" alt="Secure Tropic Click" width="20%"/>

Samples can be requested through our website. Visit [tropicsquare.com/tropic01-samples](https://tropicsquare.com/tropic01-samples) for more info.


#### Breakout PCB with TROPIC01

PCB modules with `TROPIC01 engineering samples` are available as **Secure Tropic Click** through our external partner **MIKROE**'s webshop.

To order, visit [mikroe.com](https://www.mikroe.com/secure-tropic-click).

<img src="secure-tropic-click.png" alt="Secure Tropic Click" width="20%"/>

You can interface **Secure Tropic Click** with your platform over SPI bus using a breadboard or wires. Additional conversion PCBs are available on the MIKROE website, including an [arduino](https://www.mikroe.com/arduino-uno-click-shield) form factor extension board.

#### USB dongle with TROPIC01

Designed for evaluation on systems where SPI is not available.

<img src="ts1301_top_assembled.png" alt="Secure Tropic Click" width="30%"/>


Please visit [tropicsquare.com](https://tropicsquare.com/tropic01-samples), sign-up and check availability.


## Libtropic library examples and tests {#libtropic-library-examples}

Check out `examples/` folder, it contains a few examples of how libtropic functions might be used.

There is also folder `tests/functional/` with code used internally for testing.


**Functions from `examples/` and `tests/functional/` are not compiled into libtropic library by default. In order to have access to all examples functions from parent project, special switch must be passed to compilation.**

The purpose of this is to control inclusion of code, because example code could occupy some unnecessary space and it is not needed anymore once users get familiar with library - then this code can be switched off.

Examples code can be enabled by:
* pass `-DLT_BUILD_EXAMPLES=1` during parent project compilation, or
* in parent CMake file, switch this option on: `set(LT_BUILD_EXAMPLES ON)`

Functional tests code can be enabled by:
* pass `-DLT_BUILD_TESTS=1` during parent project compilation, or
* in parent CMake file, switch this option on: `set(LT_BUILD_TESTS ON)`

Both examples and functional tests require SH0 private key to establish a secure session with TROPIC01. CMake variable `LT_SH0_PRIV_PATH` is used for that and its default value is set to path to the SH0 private key from the currently used `lab_batch_package`, found in `libtropic/provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`. Naturally, it can be overridden with another path to SH0 private key. CMake loads the key raw data and saves it to `sh0priv` C array, defined in `include/libtropic_functional_tests.h` or `include/libtropic_examples.h`. The variable `LT_SH0_PRIV_PATH` is not expected to be used in user applications.

## Standalone example projects {#standalone-example-projects}

Standalone example projects are ready to be compiled and used as a starting point to build some other applications.

The consist of two parts:
* platform related project parts - dependencies, libraries, hw initialization, ...
* Code in main(), where particular libtropic example is executed

For more info check one of projects for platforms of our choice:

#### STM32

Integration example for STM32 is available through [this](https://github.com/tropicsquare/libtropic-stm32) repository and contains code for:
- Nucleo-f439zi
- Nucleo-l432kc

#### Unix

For Unix based examples we have `cmd line utility` called [libtropic-util](https://github.com/tropicsquare/libtropic-util).

This is Unix program to test API calls against TROPIC01, works for:
- TROPIC01 connected over SPI - tested on Raspberrypi 3 and 4
- USB dongle with TROPIC01

## Adding libtropic to existing project {#adding-libtropic-to-existing-project}

How can you add libtropic to your existing project?

At first please read all chapters in this page so you get familiar with how library works.

Then we recommend to add libtropic as a submodule. Libtropic uses CMAKE build system, therefore it could be added to compilation of existing CMake projects in a following way:

```
# CMakeLists.txt file

# Set path of libtropic submodule
set(PATH_LIBTROPIC "libtropic/")

# This switch will expose not only core library functions, but also helper functions (recommended)
set(LT_HELPERS ON)

# This switch exposes also functions and tests containing example of usage.
# Might be a good starting point.
#set(LT_BUILD_EXAMPLES ON)
#set(LT_BUILD_TESTS ON)

# It is necessary to set provider of cryptography functions
# Use trezor crypto as a source of backend cryptography code
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

### Linking

target_link_options(produced_binary PRIVATE -Wl,--gc-sections)

```

Please note that exact CMake calls depend on configuration of a project into which libtropic is being added. For more inspiration have a look into standalone example projects.


#### Do you use Makefile instead of CMake?

In this case you have to add list of all libtropic *.c and *.h files manually to your makefile and then for all CMake ON option (located in libtropic's CMakeFile.txt) you define -D switch in your makefile.

Then do the same for files used in trezor_crypto.


## Using CMAKE switches {#using-cmake-switches}

Libtropic is configurable with CMake options. They are either passed from parent project's CMakeLists.txt, or over prompt when building is invoked.

```
option(LT_USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(LT_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)
option(LT_BUILD_DOCS "Build documentation" OFF)
option(LT_BUILD_EXAMPLES "Compile example code as part of libtropic library" OFF)
option(LT_BUILD_TESTS "Compile functional tests' code as part of libtropic library" OFF)
option(LT_ENABLE_FW_UPDATE "Enable firmware update functions and compile firmware update in a form of byte array" OFF)
# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only basic libtropic API.
option(LT_HELPERS "Compile helper function" ON)
# Enable usage of INT pin during communication. Instead of polling for response,
# host will be notified by INT pin when response is ready.
option(LT_USE_INT_PIN "Use INT pin instead of polling for TROPIC01's response" OFF)
option(LT_SEPARATE_L3_BUFF "Define L3 buffer separately out of the handle" OFF)
option(LT_PRINT_SPI_DATA "Print SPI communication to console, used to debug low level communication" OFF)
```
