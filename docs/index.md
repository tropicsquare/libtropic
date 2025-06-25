# Hello World!

Welcome to the documentation page for **libtropic**, the official C library for TROPIC01 chip. This guide will help you get started with building applications using **TROPIC01** and **libtropic**.

For detailed information about the libtropic library architecture and development, please refer to the `For contributors` section.

- [Getting TROPIC01](#getting-tropic01)
- [Standalone example projects](#standalone-example-projects)
- [Libtropic library examples](#libtropic-library-examples)
- [Adding libtropic to existing project](#adding-libtropic-to-existing-project)
- [Using CMAKE switches](#using-cmake-switches)
- [More examples](#more-examples)

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


## Libtropic library examples {#libtropic-library-examples}

Check out `examples/` folder, it contains a few examples of how libtropic functions might be used.

We provide following examples:

- `lt_ex_hello_world.c`: Demonstrates basic usage of libtropic, good starting point
- `lt_ex_fw_update.c`: Shows how to update TROPIC01's internal firmware
- `lt_ex_hw_wallet.c`: Example of how a generic hw wallet project might integrate TROPIC01

There is also folder `tests/functional/` with code used internally for testing, so you can get some inspiration:
- `lt_test_reversible`: This routine causes changes which can be reverted
- `lt_test_ireversible.c`: This routine causes changes which cannot be reversed
- `lt_test_samples_1.c`: This routine cauees changes which cannot be reversed. Used for testing samples.


**Functions from `examples/` and `tests/functional/` are not compiled into libtropic library by default. In order to have access to all examples functions from parent project, special switch must be passed to compilation.**

The purpose of this is to control inclusion of code, because example code could occupy some unnecessary space and it is not needed anymore once users get familiar with library - then this code can be switched off.

Examples code can be enabled by:
* pass `-DLT_ADD_EXAMPLES=1` during parent project compilation, or
* in parent CMake file, switch this option on : `set(LT_ADD_EXAMPLES ON)`

Functional tests code can be enabled by:
* pass `-LT_BUILD_TESTS=1` during parent project compilation, or
* in parent CMake file, switch this option on : `set(LT_BUILD_TESTS ON)`

When libtropic is compiled like mentioned,  examples(tests) are then available in parent project like this:

```
#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"

int main(void)
{
    // This is hello world from example folder
    lt_ex_hello_world();
    // This is test function from tests/functional/ folder
    lt_test_reversible();
}
```

This organization allows our examples and test functions to be platform agnostic.


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
#set(LT_ADD_EXAMPLES ON)
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

option(LT_USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(LT_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)
option(LT_BUILD_DOCS "Build documentation" OFF)
option(LT_ADD_EXAMPLES "Compile example code as part of libtropic library" OFF)
option(LT_BUILD_TESTS "Compile functional tests' code as part of libtropic library" OFF)
option(LT_ENABLE_FW_UPDATE "Enable firmware update functions and compile firmware update in a form of byte array" OFF)

```
# Set trezor_crypto as a cryptography provider
-DLT_USE_TREZOR_CRYPTO=1

# Compile libtropic also with functions containing example usage - good for initial evaluation
-DLT_ADD_EXAMPLES=1

# Compile functional tests' code as part of libtropic library
-DLT_BUILD_TESTS=1

# Enable firmware update functions and compile firmware update in a form of byte array
# For more info have a look into examples/fw_update.c
-DLT_ENABLE_FW_UPDATE=1

# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only libtropic's core
-DLT_HELPERS=1

# Build libtropic documentation
-DLT_BUILD_DOCS=1

# Use assert() in macros LT_ASSERT and LT_ASSERT_COND (handy for the Unix port)
-DLT_USE_ASSERT=1
```
