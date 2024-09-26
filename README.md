# libtropic

![](https://github.com/tropicsquare/libtropic/actions/workflows/unit_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/integration_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/build_docs.yml/badge.svg) ![](https://tropic-gitlab.corp.sldev.cz/internal/sw-design/libtropic/badges/master/coverage.svg)

TROPIC01's SDK written in C.

Content of this repository is licensed under "Apache 2.0 License”, the copy of the license can be obtained here: https://www.apache.org/licenses/LICENSE-2.0.txt.

For more info about TROPIC01 check datasheet and API pdf.


# Repository structure
* `docs/` Documentation
* `examples/` A few examples how libtropic might be used
* `hal/` Support code for various platforms - transport layer, delay function and RNG
* `include/` Public API header files
* `src/` Library's source files
* `tests/` Unit tests and integration tests
* `vendor/` Third party libraries and tools

# Get started!

Instructions differ based on whether you have physical chip on hand or not:

### Physical TROPIC01

[This](phttps://github.com/tropicsquare/libtropic-stm32) link leads to an example embedded project, featuring `STM32` devboard and `real TROPIC01` chip.

### Emulated TROPIC01

[This](tests/integration/integration_tests.md) link leads to an example project, featuring `Unix` environment and `emulated TROPIC01` chip.

# Documentation

More info can be found here: `docs/index.md`
