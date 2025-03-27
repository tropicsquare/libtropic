# Hello World!

Welcome to the documentation page for **libtropic**, the official C library for TROPIC01 chip. This guide will help you get started with building applications using **TROPIC01** and **libtropic**.

For detailed information about the libtropic library architecture and development, please refer to the contributors section.

- [Getting TROPIC01](#getting-tropic01)
- [Integration examples](#integration-examples)
- [Adding libtropic](#adding-libtropic)
- [Using CMAKE switches](#using-cmake-switches)
- [More examples](#more-examples)

## Getting TROPIC01 {#getting-tropic01}

TROPIC01 is currently available in three forms:

- QFN32 4x4mm IC package for direct PCB integration
- Development board with pre-soldered TROPIC01 for easy evaluation
- USB dongle version for systems without SPI bus

#### QFN32 Samples


<img src="QFN32.jpg" alt="Secure Tropic Click" width="20%"/>

Samples can be requested through our website. Visit [tropicsquare.com/tropic01-samples](https://tropicsquare.com/tropic01-samples) for more info.


#### Breakout PCB with TROPIC01

PCB modules are available as **Secure Tropic Click** through our external partner **MIKROE**'s webshop.

To order, visit [mikroe.com](https://www.mikroe.com/secure-tropic-click).

<img src="secure-tropic-click.png" alt="Secure Tropic Click" width="20%"/>

You can interface **Secure Tropic Click** with your platform over SPI bus using a breadboard or wires. Additional conversion PCBs are available on the MIKROE website, including an [arduino](https://www.mikroe.com/arduino-uno-click-shield) form factor extension board.

#### USB dongle with TROPIC01

Designed for evaluation on systems where SPI is not available.

<img src="ts1301_top_assembled.png" alt="Secure Tropic Click" width="30%"/>


Please visit [tropicsquare.com](https://tropicsquare.com/tropic01-samples), sign-up and check availability.


## Integration examples {#integration-examples}

We provide standalone compilable example projects for platforms of our choice:

#### STM32

Integration example for STM32 is available through [this](https://github.com/tropicsquare/libtropic-stm32) repository and contains code for:
- Nucleo-f439zi
- Nucleo-l432kc

#### Unix

For unix based examples we have `cmd line utility` called [libtropic-util](https://github.com/tropicsquare/libtropic-util).

This is Unix program to test API calls against TROPIC01, works for:
- TROPIC01 connected over SPI - tested on Raspberrypi 3 and 4
- USB dongle with TROPIC01

## Adding libtropic {#adding-libtropic}

How can you add libtropic to your existing project?

We recommend to add libtropic as a submodule. Libtropic uses CMAKE build system, therefore it could be added to compilation of existing cmake projects in a following way:

```
# CMakeLists.txt file

# Set path of libtropic submodule
set(PATH_LIBTROPIC "libtropic/")

# This switch will expose not only core library functions, but also helper functions
set(LT_HELPERS ON)

# This switch exposes also functions containing example of usage.
# Might be a good starting point.
#set(LT_ADD_EXAMPLES ON)

# It is necessary to set provider of cryptography functions
# Use trezor crypto as a source of backend cryptography code
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

### Linking

target_link_options(lt-util PRIVATE -Wl,--gc-sections)

```

Please note that exact cmake calls depend on configuration of a project into which libtropic is being added. For more inspiration have a look into integration examples.

## Using CMAKE switches {#using-cmake-switches}

Libtropic is configurable with cmake options. They are either passed from parent project's CMakeLists.txt, or over prompt when building is invoked.

```
# Set trezor_crypto as a cryptography provider
-DLT_USE_TREZOR_CRYPTO=1

# Compile libtropic also with functions containing example usage - good for initial evaluation
-DLT_ADD_EXAMPLES=1

# Enable firmware update functions and compile firmware update in a form of byte array
# For more info have a look into examples/fw_update.c
-DLT_ENABLE_FW_UPDATE=1

# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only libtropic's core
-DLT_HELPERS=1

# Build libtropic documentation
-DLT_BUILD_DOCS=1
```

## More examples {#more-examples}

We provide following examples:

1. `hello world`: Demonstrates basic usage of libtropic.
2. `fw update`: Shows how to update TROPIC01's internal firmware
3. `hw wallet`: Example of how a generic hw wallet project might integrate TROPIC01
4. `Mac and Destroy`: Example of usage of Mac and Destroy pin verification engine

In order to have examples exposed in your parent project, libtropic must be compiled with following CMake switches:

```
# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only libtropic's core
-DLT_HELPERS=1

# Compile libtropic also with functions containing example usage - good for initial evaluation
-DLT_ADD_EXAMPLES=1
```
