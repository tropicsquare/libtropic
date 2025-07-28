# Testing and running examples against TROPIC01 model
Code in this directory is meant to be compiled under Unix and all tests and examples here are running against TROPIC01's Python model, so no chip or external hardware is needed.

- Testing is managed by CTest: the CTest will execute both test and model automatically.
- For running examples, you need to start the model manually and then execute the example binary in a separate terminal. See below.

## How it works?
Both processes (tests and model) will talk to each other through TCP socket at 127.0.0.1:28992. The SPI layer between libtropic and model is emulated through this TCP connection. The model responses are exactly the same as from physical TROPIC01 chip.

## Model setup
First, the model has to be installed. For that, follow the readme in the [ts-tvl](https://github.com/tropicsquare/ts-tvl) repository.

Next, before running CTest, it is advised to initialize the model with some data, so it can behave like the real provisioned chip. The model configuration has to be put into the file `model_cfg.yml` (see [here](https://github.com/tropicsquare/ts-tvl#model-configuration) for information about the model configuration). It is advised to use the [`create_model_cfg.py`](create_model_cfg.py) script - it generates the model configuration based on `lab_batch_package`, containing provisioning data used in the Tropic Square lab for testing purposes. The script is used as:
```shell
python3 create_model_cfg.py --pkg-dir <path_to_the_lab_batch_package_directory>
```

Lab batch packages are contained in `libtropic/provisioning_data/`.

Model logging can also be configured in the file [model_logging.cfg](model_logging_cfg.yml) (see [documentation](https://docs.python.org/3/library/logging.config.html) of the Python `logging` module).

## Compilation
To compile all the examples, in the `tests/model_based_project` directory, run:
```sh
mkdir build
cd build
cmake -DLT_BUILD_EXAMPLES=1 ..
make
```

To compile all the tests, in the `tests/model_based_project` directory, run:
```sh
mkdir build
cd build
cmake -DLT_BUILD_TESTS=1 ..
make
```

Notes:
- Binaries will be compiled to the `tests/model_based_project/build` directory.
- You can of course combine the CMake flags and compile everything at once:
`cmake -DLT_BUILD_TESTS=1 -DLT_BUILD_EXAMPLES=1 ..`.
- To include debug symbols, also add `-DCMAKE_BUILD_TYPE=Debug` CMake flag.
- We also provide convenience script for test compilation (primarily used in CI): [`compile_tests.sh`](compile_tests.sh).
- When `LT_BUILD_TESTS` is set, there has to be a way to define the SH0 private key, as the tests need it to establish a secure session. CMake variable `LT_SH0_PRIV_PATH` is used for that and its default value is set to path to the SH0 private key from the currently used `lab_batch_package`, found in `libtropic/provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`.

## Running the tests
The tests are managed by CTest. The CTest will run both model and the test executable automatically.

First, switch to the `tests/model_based_project/build` directory. To see available tests, run
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

After CTest finishes, it informs about the results and saves all output to the `tests/model_based_project/build/run_logs/` directory. Output from the tests and responses from the model are saved.
> Note: the model is automatically started for each test separately, so it behaves like a fresh TROPIC01 straight out of factory. All this and other handling is done by the script [model_test_runner.py](model_test_runner.py), which is called by CTest.

## Running the examples
Examples are meant to be executed manually:
1. Activate the virtual environment you created during [model setup](#model-setup) if not already active.
2. Start the model: `model_server tcp --configuration=model_cfg.yml`
   - `model_cfg.yml` is a config file you created during [model setup](#model-setup).
3. In another terminal, execute a desired example. For example: `build/lt_ex_hello_world`.

In this case, all output is logged to stdout.