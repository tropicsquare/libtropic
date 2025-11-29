# Libtropic

![](https://github.com/tropicsquare/libtropic/actions/workflows/integration_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/build_docs_master.yml/badge.svg)

TROPIC01's SDK written in C. Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic/blob/master/CONTRIBUTING.md).

For more information about TROPIC01 chip and its **datasheet** or **User API**, check out developers resources in the [TROPIC01](https://github.com/tropicsquare/tropic01) repository.

## Documentation
We recommend using the [Libtropic documentation](https://tropicsquare.github.io/libtropic/latest/) as the source of truth for getting information about Libtropic.

The default documentation version is generated from the latest version of the master branch, but release versions are also available (via the version selector at the top of the page).

## Compatibility with TROPIC01 firmware versions

For the Libtropic library to function correctly with the TROPIC01 secure element, the versions of four key components must be compatible:

1. **Libtropic SDK**: The version of this library.
2. **Bootloader FW**: Bootloader firmware running on the TROPIC01's RISC-V CPU after power-up. It cannot be updated.
2. **Application FW**: Application firmware running on the TROPIC01's RISC-V CPU. It can be updated.
3. **SPECT FW**: Firmware running on the TROPIC01's SPECT co-processor. It can be updated.

The following table outlines the tested and supported compatibility between released versions:

| Libtropic         | Application FW          | SPECT FW            | Bootloader FW |  Tests                                     | API document version                                                                                   |
|:-------------------:|:----------------------:|:----------------:|  :-------:  |  :---------------------------------------: | :----------------------------------------------------------------------------------------------------: |
| 1.0.0               | 1.0.0                  | 1.0.0            |  1.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf)       |
| 2.0.0               | 1.0.0–1.0.1                  | 1.0.0            |  2.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf)       |
| 2.0.1               | 1.0.0–1.0.1                  | 1.0.0            |  2.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf) |
| 3.0.0               | 1.0.0–2.0.0                  | 1.0.0            |  2.0.1     |  <code style="color : green">Passed</code> | [1.3.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.3.0.pdf), [1.4.0](https://github.com/tropicsquare/tropic01/blob/main/doc/api/ODU_TR01_user_api_v1.4.0.pdf) |

Using mismatched versions of the components may result in unpredictable behavior or errors. It is strongly advised to use the latest compatible versions of all components to ensure proper functionality.  

For retrieving firmware versions from TROPIC01, check the [lt_ex_show_chip_id_and_fw_ver](https://tropicsquare.github.io/libtropic/latest/get_started/examples/reversible_examples/#lt_ex_show_chip_id_and_fwverc) example. For updating firmware, please follow code example in [lt_ex_fw_update](https://tropicsquare.github.io/libtropic/latest/get_started/examples/irreversible_examples/#lt_ex_fw_updatec).

## Repository structure
* `CMakeLists.txt` Root CMake project file
* `cmake/` CMake related files
* `cal/` Implementation of Crypto Abstraction Layers (CAL) for supported Cryptographic Functionality Providers (CFP)
* `docs/` [MkDocs](https://www.mkdocs.org/) Documentation deployed [here](https://tropicsquare.github.io/libtropic/latest/)
* `examples/` A few examples of how Libtropic might be used
* `hal/` Implementation of Hardware Abstraction Layers (HAL) for supported host platforms
* `include/` Public API header files
* `scripts/` Build and config scripts
* `src/` Library's source files
* `tests/` Functional tests
* `TROPIC01_fw_update_files/` Files used for updating TROPIC01's firmware
* `tropic01_model/` Project for the TROPIC01's model, utilizing the TCP HAL
* `vendor/` Third party libraries and tools

## Get started!

Platform repositories, showing example usage of libtropic on known platforms:
* [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32)
* [libtropic-linux](https://github.com/tropicsquare/libtropic-linux)
* [libtropic-arduino](https://github.com/tropicsquare/libtropic-arduino)
* [libtropic-util](https://github.com/tropicsquare/libtropic-util)

## FAQ
We provide the [FAQ](https://tropicsquare.github.io/libtropic/latest/faq/) section in our documentation with frequently asked questions and troubleshooting tips.

## License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](http:/tropicsquare.com/license).

