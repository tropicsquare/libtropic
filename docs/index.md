# Introduction

`libtropic` is a C library, which implements functionalities for interfacing applications with TROPIC01 device. It comes without any security claims and shall be used for evaluation purpose only.


|Supported devices                                       |Description                                              |
|--------------------------------------------------------|---------------------------------------------------------|
|[TROPIC01](https://www.tropicsquare.com/tropic01)       | First generation TROPIC01                               |

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
To build html documentation, you need Doxygen. The documentation is built using these commands:
```sh
$ mkdir build/
$ cd build/
$ cmake -DBUILD_DOCS=1 ..
$ make doc_doxygen
```

The documentation will be built to: `build/docs/doxygen/html`. Open `index.html` in any recent web browser.

Notes:
- We tested Doxygen 1.9.1 and 1.9.8.
- PDF (LaTeX) output is not supported.

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
For now we support Ceedling version 0.31.1 only.

Once ceedling is installed, run tests and create code coverage report:

```
$ CEEDLING_MAIN_PROJECT_FILE=scripts/ceedling.yml ceedling gcov:all utils:gcov
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

## Static code analysis
The CodeChecker tool is used to do static code analysis and generate reports.

There are 3 options how to get reports:
1. Download pre-generated HTML report artifact from GitHub Actions.
    This is the simplest way. Reports are generated on every push to develop branch for now.
2. Generate HTML report yourself.
    - You need to install CodeChecker (e.g. via pip). After that, run:
    ```sh
    CodeChecker check -b "./scripts/codechecker/codechecker_build.sh" --config ./scripts/codechecker/codechecker_config.json --output ./.codechecker/reports
    CodeChecker parse -e html ./.codechecker/reports -o ./.codechecker/reports_html
    ```
    - Open `./.codechecker/reports_html/index.html` in your browser.
3. Use CodeChecker add-on.
    - Add these lines to your VS Code's workspace settings (`.vscode/settings.json`):
    ```json
    "codechecker.executor.executablePath": <path to CodeChecker>,
    "codechecker.executor.arguments": "--config scripts/codechecker/codechecker_config.json",
    "codechecker.executor.logBuildCommand": "./scripts/codechecker/codechecker_build.sh",
    "codechecker.backend.compilationDatabasePath": "${workspaceFolder}/.codechecker/compile_commands.json"
    ```
    - `<path to CodeChecker>` can be replaced with "CodeChecker" if the CodeChecker is available in your `$PATH`. Otherwise, you need to specify full path to the CodeChecker executable.
