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
  - [Adding libtropic to existing project {#adding-libtropic-to-existing-project}](#adding-libtropic-to-existing-project-adding-libtropic-to-existing-project)
      - [Do you use Makefile instead of CMake?](#do-you-use-makefile-instead-of-cmake)

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


> [!NOTE]
Examples or functional tests are not compiled by default. They can be compiled by adding special CMake options, which is discussed further in this text.

To compile examples, either
- pass `-DLT_BUILD_EXAMPLES=1` to `cmake` during compilation, or
- in your CMake file, switch this option on: `set(LT_BUILD_EXAMPLES ON)`.

To compile functional tests, either
- pass `-DLT_BUILD_TESTS=1` to `cmake` during compilation, or
- in your CMake file, switch this option on: `set(LT_BUILD_TESTS ON)`.

> [!IMPORTANT]
> When `LT_BUILD_EXAMPLES` or `LT_BUILD_TESTS` are set, there has to be a way to define the SH0 private key for the TROPIC01's pairing key slot 0, because both the examples and the tests depend on it. For this purpose, the CMake variable `LT_SH0_PRIV_PATH` is used, which should hold the path to the file with the SH0 private key in PEM or DER format. By default, the path is set to the currently used lab batch package, found in `../provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`. But it can be overriden by the user either from the command line when executing CMake (switch `-DLT_SH0_PRIV_PATH=<path>`), or from a child `CMakeLists.txt`.

> [!TIP]
> For more information about the `provisioning_data/` directory structure, see it's README.md.

> [!TIP]
> To see the whole process of executing examples or functional tests against the TROPIC01 model, see README.md in the `tropic01_model/` directory.

## Standalone example projects {#standalone-example-projects}
Our examples and functional tests are platform independent, so they can also be compiled and run in our platform repositories:
1. [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32)
2. [libtropic-linux](https://github.com/tropicsquare/libtropic-linux)

Both of these repositories consist of:
- libtropic as a git submodule
- directories for supported platforms
  - `CMakeLists.txt`
  - include directory
  - source directory with `main.c`
  - additional files (readme, scripts, ...)

For more detailed info, refer to the aforementioned platform repositories.

Besides that, we offer the [libtropic-util](https://github.com/tropicsquare/libtropic-util) repository, which implements a CLI based utility for executing TROPIC01's commands.

## Adding libtropic to existing project {#adding-libtropic-to-existing-project}

How can you add libtropic to your existing project?

At first please read all chapters in this page so you get familiar with how library works.

Then we recommend to add libtropic as a submodule. Libtropic uses CMake build system, therefore it could be added to compilation of existing CMake projects in a following way:

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

target_link_options(produced_binary PRIVATE <your_linker_flags>)

```

> [!NOTE]
Exact CMake calls depend on configuration of a project into which libtropic is being added. For more inspiration, have a look at the aforementioned platform repositories.

> [!TIP]
> We offer multiple CMake options - to see all of them, go to the beginning of the `CMakeLists.txt` file in the repository's root directory.


#### Do you use Makefile instead of CMake?

In this case you have to add list of all libtropic *.c and *.h files manually to your makefile and then for all CMake ON options (located in libtropic's CMakeFile.txt) you define -D switch in your makefile.

Then do the same for files used in `vendor/trezor_crypto/`.