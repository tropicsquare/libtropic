# Tests folder

Describe various ways of how testing works here and what is needed to run it TODO improve

### Functional

This folder contains C code for functional tests used internally in Topic Square.

# model_based_project

This is a C project which is configured to compile libtropic with TCP transport layer, so the produced binary can be executed against python model of TROPIC01.
The purpose of this project is that this configuration simplifies development, because no real chip is needed.

This executable is also used in CI job when checking pull requests. TODO

# Unit

Unit tests with a help of ceedling testing framework

