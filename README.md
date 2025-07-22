# libtropic

![](https://github.com/tropicsquare/libtropic/actions/workflows/unit_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/integration_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/build_docs.yml/badge.svg) ![](https://tropic-gitlab.corp.sldev.cz/internal/sw-design/libtropic/badges/master/coverage.svg)

TROPIC01's SDK written in C. Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic/blob/master/CONTRIBUTING.md).


For more info about TROPIC01 chip check out developers resources in [TROPIC01](https://github.com/tropicsquare/tropic01) repository.


# Repository structure
* `docs/` Documentation
* `examples/` A few examples how libtropic might be used
* `hal/` Support code for various platforms - transport layer, delay function and RNG
* `include/` Public API header files
* `keys/` Default pairing keys for slots 1, 2, 3 (used in examples and functional tests)
* `provisioning_data/` Contains pairing key for slot 0; this data can be used to "provision" the TROPIC01 model (more info [here](tests/model_based_project/README.md))
* `scripts/` Build and config scripts
* `src/` Library's source files
* `tests/` Functional and unit tests, along with a project for testing against the TROPIC01 model
* `vendor/` Third party libraries and tools

# Get started!

For engineering samples please go [here](https://tropicsquare.com/tropic01).

Example projects:

* [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32) - example project featuring `STM32 NUCLEO_F439ZI` or `STM32 NUCLEO_L432KC`
* [libtropic-util](https://github.com/tropicsquare/libtropic-util) - example project featuring `USB Dongle with TROPIC01` or `Raspberry Pi`


# Documentation

HTML documentation for the latest master version is available [here](https://tropicsquare.github.io/libtropic/).

The documentation can be built manually as following:
```sh
mkdir build
cmake -DBUILD_DOCS=1 ..
make doc_doxygen
```

The documentation will be available in the `docs/doxygen/html`.
Open [index.html](build/docs/doxygen/html/index.html) with your favorite web browser.

# License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](http:/tropicsquare.com/license).

