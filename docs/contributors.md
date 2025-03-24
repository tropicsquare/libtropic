# For contributors

This section is for those who want to do something with the library itself.

- [Library's architecture](#library-architecture)
- [How to: Adding new embedded platform](#adding-new-embedded-platform)
- [How to: Adding new cryptographic backend](#adding-new-cryptographic-backend)
- [How to: Compile as static archive](#compile-as-static-archive)
- [How to: Run unit tests](#run-unit-tests)
- [How to: Run static analysis](#run-static-analysis)
- [How to build documentation](#build-documentation)

## Library's architecture {#library-architecture}

Library architecture explained: core, examples, helpers, layers, hal, …

“Libtropic is standalone library expected to be used within a parent project”

TBD add picture

## How to: Adding new embedded platform {#adding-new-embedded-platform}

To add a new embedded platform, follow these steps:
1. Implement the necessary HAL (Hardware Abstraction Layer) functions, according to `include/libtropic_port.h`
2. Test the integration by compiling hello world example within your project

For more inspiration check out our Integration examples.

## How to: Adding new cryptographic backend {#adding-new-cryptographic-backend}

As the libtropic was designed with extensibility in mind, it is possible to add a new backend
for cryptographic operations.

The new backend has to support operations with:
- AES (GCM)
    - encryption
    - decryption
- ED25519
    - signing
- SHA256
    - hashing
- Curve25519
    - multiplication on both arbitrary and base point

#### Guide

To add a new backend (let's say mycrypto), you need to:
1. Create a new directory in `hal/crypto` called `mycrypto`.
2. Create 4 source files in this directory:
    - `lt_crypto_mycrypto_aesgcm.c`
    - `lt_crypto_mycrypto_ed25519.c`
    - `lt_crypto_mycrypto_sha256.c`
    - `lt_crypto_mycrypto_x25519.c`
3. In each of the source files, implement all required functions. The functions are declared in respective headers in `src/`:
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_ed25519.h`: ED25519 functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    - Look into each of the headers. Purpose of every function is described in its comment. Copy function declarations
    from the headers to the sources and implement the functions. For exmple, to implement Curve25519 functions,
    I will copy declarations from the `lt_x25519.h` to `lt_crypto_mycrypto_x25519.c` and provide implementations.
    You can use existing ports for inspiration.

4. Edit main CMakeLists --> see following section.
5. Done. The new crypto backend can be enabled by passing `-DLT_CRYPTO_MYCRYPTO` to the CMake.

#### Editing CMakeLists

In the Setup section, add new option:
```cmake
option(LT_CRYPTO_MYCRYPTO "Use mycrypto as a cryptography provider" OFF)
```

At the bottom of the Setup section, there are checks for definition of cryptographic backend. Add your new backend to the checks:

```cmake
# Check if cryptography provider is defined
# Check if cryptography provider is defined
if(
    (NOT BUILD_DOCS)         AND
    (NOT USE_TREZOR_CRYPTO)  AND
    (NOT LT_CRYPTO_MBEDTLS)  AND
    (NOT LT_CRYPTO_MYCRYPTO)     # <-- YOUR NEW OPTION HERE
)
    message(FATAL_ERROR "No cryptography provider is defined.")
endif()
```

In the Collect files section, add the new implementation files to compilation. Use this template,
modify it accordingly, and paste it above the `add new crypto sources above this line` comment:

```cmake
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_x25519.c
)
```

At last, in the Compile and link section, compile and link dependencies to your new crypto backend:

```cmake
if(LT_CRYPTO_MYCRYPTO)
    target_compile_definitions(tropic PRIVATE LT_CRYPTO_MYCRYPTO)
    add_subdirectory(<edit me>) # Edit or remove.
    target_link_libraries(tropic PRIVATE <edit me>) # Edit or remove.
    # Add more lines as neccessary.
endif()
```

## How to: Compile as static archive {#compile-as-static-archive}

*Note: When compiling the library standalone as a static archive, a cryptography provider must be defined through cmake -D arguments*

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
## How to: Run unit tests {#run-unit-tests}

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
For now we support Ceedling version 0.31.1 only.

Once ceedling is installed, run tests and create code coverage report:

```
$ CEEDLING_MAIN_PROJECT_FILE=scripts/ceedling.yml ceedling gcov:all utils:gcov
```

#### Randomization
Some tests use a rudimentary randomization mechanism using standard C functions. The PRNG is normally
seeded with current time. Used seed is always printed to stdout. You can find the seed in logs
(`build/gcov/results/...`).

To run tests with fixed seed, set `RNG_SEED` parameter to your
desired seed (either directly in the file, or in the project.yml section `:defines:`). This is
useful mainly to replicate a failed test run -- just find out what seed was used and then
set `RNG_SEED` to this.

## How to: Run static analysis {#run-static-analysis}

To run static analysis, follow these steps:
1. Choose a static analysis tool (e.g., cppcheck, clang-tidy).
2. Configure the tool to analyze the library code.
3. Run the analysis and review the reported issues.

## How to build documentation {#build-documentation}

To build html documentation, you need Doxygen. The documentation is built using these commands:
```sh
$ mkdir build/
$ cd build/
$ cmake -DLT_BUILD_DOCS=1 ..
$ make doc_doxygen
```

The documentation will be built to: `build/docs/doxygen/html`. Open `index.html` in any recent web browser.

Notes:
- We tested Doxygen 1.9.1 and 1.9.8.
- PDF (LaTeX) output is not supported.