# Tests
All tests are implemented in the `tests/` directory. There are two groups of tests:

1. [Functional Tests](functional_tests.md) (`tests/functional/`): tests for verifying the libtropic core API,
2. [Unit Tests](unit_tests.md) (`tests/unit/`): basic checking of function arguments and return values using Ceedling, do not test correct functionality.

In the `libtropic` repository, tests are run against the TROPIC01 model only - refer to [TROPIC01 Model](../other/tropic01_model.md) section for more information. Testing aganist the TROPIC01 model is also utilized in a CI job, triggered for the `master` and `develop` branches (pushes and pull requests).

The tests can also be run in the [libtropic platform repositories](https://github.com/tropicsquare/libtropic#get-started).

