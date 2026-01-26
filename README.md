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

For more information about each of these, refer to the [TROPIC01](https://github.com/tropicsquare/tropic01) repository.

The following table outlines the tested and supported compatibility between released versions:

| Libtropic | Application FW | SPECT FW | Bootloader FW |  Tests                                    |
|:---------:|:--------------:|:--------:|:-------------:|:----------------------------------------: |
| 1.0.0     | 1.0.0          | 1.0.0    | 1.0.1-2.0.1   | <code style="color : green">Passed</code> |
| 2.0.0     | 1.0.0–1.0.1    | 1.0.0    | 2.0.1         | <code style="color : green">Passed</code> |
| 2.0.1     | 1.0.0–1.0.1    | 1.0.0    | 2.0.1         | <code style="color : green">Passed</code> |
| 3.0.0     | 1.0.0–2.0.0    | 1.0.0    | 2.0.1         | <code style="color : green">Passed</code> |

> [!WARNING]
> Using mismatched versions of the components may result in unpredictable behavior or errors. It is strongly advised to use the latest compatible versions of all components to ensure proper functionality. 

For retrieving firmware versions from TROPIC01 and updating its firmware, refer to the [Tutorials](https://tropicsquare.github.io/libtropic/latest/tutorials/) and select your platform. Follow the instructions for **Chip Identification** and **Firmware Update** example.

## Repository structure
* `CMakeLists.txt` Root CMake project file
* `cmake/` CMake related files
* `cal/` Implementation of Crypto Abstraction Layers (CAL) for supported Cryptographic Functionality Providers (CFP)
* `docs/` [MkDocs](https://www.mkdocs.org/) Documentation deployed [here](https://tropicsquare.github.io/libtropic/latest/)
* `examples/` Example projects for each supported platform
* `hal/` Implementation of Hardware Abstraction Layers (HAL) for supported host platforms
* `include/` Public API header files
* `scripts/` Build and config scripts
* `src/` Library's source files
* `tests/` Functional tests
* `TROPIC01_fw_update_files/` Files used for updating TROPIC01's firmware
* `vendor/` Third party libraries and tools

## FAQ
We provide the [FAQ](https://tropicsquare.github.io/libtropic/latest/faq/) section in our documentation with frequently asked questions and troubleshooting tips.

## License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](http:/tropicsquare.com/license).

