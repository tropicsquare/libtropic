# Testing
There are two types of tests used:
1. **functional tests** (`functional/`) - this folder contains C code for functional tests used internally in Topic Square,
2. **unit tests** (`unit/`) - basic checking of function arguments and return values using Ceedling, do not test correct behavior.

Moreover, the `model_based_project/` directory contains a C project which is configured to compile libtropic with TCP transport layer
and the functional tests (or examples) can be executed against the Python model of TROPIC01 (see [ts-tvl](github.com/tropicsquare/ts-tvl/)).
Testing against model simplifies development, because no real chip is needed and irreversible operation can be safely tested. Testing aganist
model is also utilized in a CI job, triggered for example on pull requests.

