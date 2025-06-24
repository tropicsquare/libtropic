# Testing against TROPIC01 model
Code in this directory is meant to be compiled under Unix and all tests here are running against TROPIC01's Python model, so no chip or external hardware is needed. All testing is managed by CTest.

## How it works?
Both processes (tests and model) will talk to each other through TCP socket at 127.0.0.1:28992. The SPI layer between libtropic and model is emulated through this TCP connection. The model responses are exactly the same as from physical TROPIC01 chip.

## Model setup
First, the model has to be installed. For that, follow the readme in the [ts-tvl](github.com/tropicsquare/ts-tvl/) repository.

Next, before running CTest, it is advised to initialize the model with some data, so it can behave like the real provisioned chip. This data can be specified in the file [model_cfg.yml](model_cfg.yml) (see [here](https://github.com/tropicsquare/ts-tvl#model-configuration) for information about the model configuration). Logging from the model can also be configured in the file [model_logging.cfg](model_logging_cfg.yml) (see [documentation](https://docs.python.org/3/library/logging.config.html) of the Python `logging` module).


## Compilation
To compile libtropic and the tests, the script [compile_tests.sh](compile_tests.sh) can be executed from this directory. It does the following:
1. removes `build/` directory, if it already exists,
2. compiles everything using CMake with the following flags:
   - `-DLT_BUILD_TESTS=1`: lets CMake know to compile the functional tests,
   - `-DCMAKE_BUILD_TYPE=Debug`: allows to use a debugger,
   - `-DLT_USE_ASSERT=1`: puts calls of `assert()` (from `assert.h`) in the macros `LT_ASSERT` and `LT_ASSERT_COND`. This is usually not used on embedded devices, but here we are using Unix port of libtropic, so it is a way to signalize that something failed in the test.
3. executes `make` in the `build/` directory.

## Running the tests
The tests are managed by CTest.

First, switch to the `build/` directory. To see available tests, run
```shell
ctest -N
```
to select specific test(s) using regex, run
```shell
ctest -R <test_regex>
```
where `<test_regex>` is a regex expression for the test names from the list. To run all tests, simply run
```shell
ctest
```
to enable verbose output, use the `-V` switch or `-W` switch for even more verbose output.

After CTest finishes, it informs about the results and saves all output to the `build/run_logs/` directory. Output from the tests and responses from the model are saved.
> Note: the model is automatically started for each test separately, so it behaves like a fresh TROPIC01 straight out of factory. All this and other handling is done by the script [model_test_runner.py](model_test_runner.py), which is called by CTest.