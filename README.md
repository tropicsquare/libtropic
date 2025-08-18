# libtropic

![](https://github.com/tropicsquare/libtropic/actions/workflows/unit_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/integration_tests.yml/badge.svg) ![](https://github.com/tropicsquare/libtropic/actions/workflows/build_docs.yml/badge.svg) ![](https://tropic-gitlab.corp.sldev.cz/internal/sw-design/libtropic/badges/master/coverage.svg)

TROPIC01's SDK written in C. Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic/blob/master/CONTRIBUTING.md).


For more info about TROPIC01 chip and it's **datasheet** or **User API**, check out developers resources in the [TROPIC01](https://github.com/tropicsquare/tropic01) repository.


## Repository structure
* `CMakeLists.txt` Root CMake project file
* `cmake/` CMake related files
* `docs/` Documentation
* `examples/` A few examples how libtropic might be used
* `hal/` Support code for various platforms - transport layer, delay function and RNG
* `include/` Public API header files
* `keys/` Default pairing keys for slots 1, 2, 3 (used in examples and functional tests)
* `provisioning_data/` Contains pairing key for slot 0; this data can be used to "provision" the TROPIC01 model (more info [here](tropic01_model/README.md) and [here](provisioning_data/README.md))
* `scripts/` Build and config scripts
* `src/` Library's source files
* `tests/` Functional and unit tests
* `TROPIC01_fw_update_files/` Files used for updating TROPIC01's firmware
* `tropic01_model/` Project for the TROPIC01's model, utilizing the Unix TCP port
* `vendor/` Third party libraries and tools

> [!TIP]
> See READMEs in some of these directories for more info.

## Get started!

Platform repositories, showing example usage of libtropic on known platforms:
* [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32)
* [libtropic-linux](https://github.com/tropicsquare/libtropic-linux)
* [libtropic-util](https://github.com/tropicsquare/libtropic-util)


## Documentation

HTML documentation for the latest master version is available [here](https://tropicsquare.github.io/libtropic/).

The documentation can be built manually as following:
```sh
mkdir build
cd build
cmake -DLT_BUILD_DOCS=1 ..
make doc_doxygen
```

The documentation will be available in the `build/docs/doxygen/html/`.
Open [index.html](build/docs/doxygen/html/index.html) with your favorite web browser.

> [!NOTE]
> After executing `cmake -DLT_BUILD_DOCS=1 ..`, you might get warnings about `LIBTROPIC_EXAMPLE_LIST` and `LIBTROPIC_TEST_LIST`. This is okay, you can ignore that.

## License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](http:/tropicsquare.com/license).

