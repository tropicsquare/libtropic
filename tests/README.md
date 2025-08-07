# Testing
There are two types of tests used:
1. **functional tests** (`functional/`) - this folder contains C code of functional tests which verify libtropic core API,
2. **unit tests** (`unit/`) - basic checking of function arguments and return values using Ceedling, do not test correct functionality.

In this repository, tests are run only against the TROPIC01 model - refer to [this](../tropic01_model/README.md) readme for more information. Testing aganist the TROPIC01 model is also utilized in a CI job, triggered for the `master` and `develop` branches (pushes and pull requests).

We also have other repositories (like [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32)), which implement a port of libtropic to specific platforms and the tests can also be run there.

