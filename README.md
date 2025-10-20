# Libtropic

![](https://github.com/tropicsquare/libtropic/actions/workflows/integration_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/build_docs_master.yml/badge.svg) ![](https://tropic-gitlab.corp.sldev.cz/internal/sw-design/libtropic/badges/master/coverage.svg)

TROPIC01's SDK written in C. Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic/blob/master/CONTRIBUTING.md).


For more information about TROPIC01 chip and its **datasheet** or **User API**, check out developers resources in the [TROPIC01](https://github.com/tropicsquare/tropic01) repository.

Libtropic documentation is available [here](https://tropicsquare.github.io/libtropic/latest/). The default version is generated from the latest version of the master branch, but release versions are also available (via the version selector at the top of the page).

## Compatibility with TROPIC01 firmware versions

For the Libtropic library to function correctly with the TROPIC01 secure element, the versions of three key components must be compatible:

1.  **Libtropic SDK**: The version of this library.
2.  **Application Firmware**: The main firmware running on the TROPIC01 chip.
3.  **SPECT Firmware**: The co-processor firmware on the TROPIC01 chip.

The following table outlines the tested and supported compatibility between released versions:

| Libtropic         | Application            | SPECT            |  Bootloader |  Tests                                     | API document version                                                                                   |
|:-------------------:|:----------------------:|:----------------:|  :-------:  |  :---------------------------------------: | :----------------------------------------------------------------------------------------------------: |
| 1.0.0               | 1.0.0                  | 1.0.0            |  v1.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf)       |
| 2.0.0               | 1.0.0–1.0.1                  | 1.0.0            |  v2.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf)       |
| 2.0.1               | 1.0.0–1.0.1                  | 1.0.0            |  v2.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf) |

Using mismatched versions of the components may result in unpredictable behavior or errors. It is strongly advised to use the latest compatible versions of all components to ensure proper functionality.  

For retrieving firmware versions from TROPIC01, check code in `examples/lt_ex_show_chip_id_and_fw_ver.c`. For updating firmware, please follow code example in `examples/lt_ex_fw_update.c`.

## Repository structure
* `CMakeLists.txt` Root CMake project file
* `cmake/` CMake related files
* `docs/` Documentation
* `examples/` A few examples of how libtropic might be used
* `hal/` Support code for various platforms - transport layer, delay function and RNG
* `include/` Public API header files
* `scripts/` Build and config scripts
* `src/` Library's source files
* `tests/` Functional tests
* `TROPIC01_fw_update_files/` Files used for updating TROPIC01's firmware
* `tropic01_model/` Project for the TROPIC01's model, utilizing the Unix TCP port
* `vendor/` Third party libraries and tools

## Get started!

Platform repositories, showing example usage of libtropic on known platforms:
* [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32)
* [libtropic-linux](https://github.com/tropicsquare/libtropic-linux)
* [libtropic-util](https://github.com/tropicsquare/libtropic-util)

## License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](http:/tropicsquare.com/license).

