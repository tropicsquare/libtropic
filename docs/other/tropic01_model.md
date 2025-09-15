# TROPIC01 Model
Code in the `tropic01_model/` directory is meant to be compiled under Unix-like OS. This directory offers running libtropic's **functional tests** and **examples** against the TROPIC01's Python model, so no chip or external hardware is needed.

- Functional testing is managed by CTest - it executes both the test and the model automatically, along with the creation of model configuration.
- When running examples, you need to start the model manually and then execute the example binary in a separate terminal. This applies also for the tests, if they are not run using CTest.
- Both of these points are discussed further in this text.

> [!WARNING]
> There are some examples which are not compatible with model, as the model does not implement all the chip's functionality. As such, those will always fail against the model. Namely:
> 
> - `lt_ex_fw_update`.

## How it Works?
Both processes (tests/examples and model) will talk to each other through TCP socket at 127.0.0.1:28992. The SPI layer between libtropic and model is emulated through this TCP connection. The model responses are exactly the same as from physical TROPIC01 chip.
> [!NOTE]
This functionality is implemented with the help of the Unix TCP HAL implemented in `hal/port/unix/lt_port_unix_tcp.c`.

## Model Setup
First, the model has to be installed. For that, follow the readme in the [ts-tvl](https://github.com/tropicsquare/ts-tvl) repository.

Next, it is possible to initialize the model with some data, so it can behave like the real provisioned chip. To do that, it is neccesary to pass a YAML configuration file to the model - see sections [Model Server](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-server) and [Model Configuration](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration) in the [ts-tvl](https://github.com/tropicsquare/ts-tvl) repository. To create such a YAML configuration, the script `tropic01_model/create_model_cfg.py` in libtropic repository is used (example usage follows).

> [!IMPORTANT]
In the case of running tests using CTest, no manual steps for creating the model configuration or initializing the model are necessary - CTest handles this by itself. In the case of running examples (or tests without CTest), the model has to be started manually by the user and some configuration has to be applied to the model, so atleast the pairing key slot 0 is written to be able to establish a secure session.

Data, from which the `tropic01_model/create_model_cfg.py` script creates the YAML configuration file for the model, can be found in `provisioning_data/` directory - see [Provisioning Data](provisioning_data.md) section for more information about the directory structure.

To create a model configuration that will initialize the model to the state which is almost identical to the provisioned chip, the `tropic01_model/create_model_cfg.py` script is run as:
```shell
cd tropic01_model/
python3 create_model_cfg.py --pkg-dir <path_to_the_lab_batch_package_directory>
```
where `<path_to_the_lab_batch_package_directory>` is the path to one of the lab batch packages inside the `provisioning_data/`. As a result of running the script, a file `model_cfg.yml` is created, which can be passed directly to the model using the `-c` flag.

## Running the Examples
1. Switch to the `tropic01_model/` directory:
```shell
cd tropic01_model/
```
2. Compile the examples:
```shell
mkdir build
cd build
cmake -DLT_BUILD_EXAMPLES=1 ..
make
```
As a result, executables for each example are built in the `tropic01_model/build/` directory.
> [!TIP]
To enable debugging symbols (e.g. to use GDB) and debug logging, add switch `-DCMAKE_BUILD_TYPE=Debug` when executing `cmake`.
>
> To use AddressSanitizer (ASan), add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_ASAN=1` when executing `cmake`.

3. Create a YAML configuration for the model from one of the lab batch packages:
```shell
python3 ../create_model_cfg.py --pkg-dir ../../provisioning_data/2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/
```
As a result, `model_cfg.yml` is created.

4. In a separate terminal, start the model server (which was previously installed in a Python virtual environment) and configure it:
```shell
model_server tcp -c model_cfg.yml
```
As a result, the model now listens on TCP port 127.0.0.1:28992.

5. In the original terminal, execute one of the built examples:
```shell
./lt_ex_hello_world
```
As a result, you should see an output from the example in the original terminal and a log from the model in the separate terminal.

## Running the Tests
> [!NOTE]
It is recommended to run the tests using CTest, but if it's needed to run the tests under GDB, they can be run exactly the same way as the examples.

1. Switch to the `tropic01_model/` directory:
```shell
cd tropic01_model/
```
2. Compile the tests in this directory:
```shell
mkdir build
cd build
cmake -DLT_BUILD_TESTS=1 ..
make
```
As a result, executables for each test are built in the `tropic01_model/build/` directory.
> [!TIP]
To enable debugging symbols (e.g. to use GDB) and debug logging, add switch `-DCMAKE_BUILD_TYPE=Debug` when executing `cmake`.
>
> To use AddressSanitizer (ASan), add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_ASAN=1` when executing `cmake`.
>
> To execute the tests with Valgrind, add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_VALGRIND=1` when executing `cmake`. Note that Valgrind will be executed automatically only when using CTest!

3. Now, the tests can be run using CTest. To see available tests, run:
```shell
ctest -N
```
To select specific test(s) using regular expression, run:
```shell
ctest -R <test_regex>
```
where `<test_regex>` is a regular expression for the test names from the list.

To run all tests, simply run:
```shell
ctest
```
> [!TIP]
To enable verbose output from CTest, run `ctest -V` or `ctest -W` switch for even more verbose output.

To exclude some tests, run:
```shell
ctest -E <test_regex>
```
where `<test_regex>` is a regular expression for the test names from the list.

After CTest finishes, it informs about the results and saves all output to the `tropic01_model/build/run_logs/` directory. Output from the tests and responses from the model are saved.
> [!NOTE]
The model is automatically started for each test separately, so it behaves like a fresh TROPIC01 straight out of factory. All this and other handling is done by the script `scripts/model_test_runner.py`, which is called by CTest.

> [!IMPORTANT]
> When `-DLT_BUILD_EXAMPLES=1` or `-DLT_BUILD_TESTS=1` are passed to CMake, there has to be a way to define the SH0 private key for the TROPIC01's pairing key slot 0, because both the examples and the tests depend on it. For this purpose, the CMake variable `LT_SH0_PRIV_PATH` is used, which should hold the path to the file with the SH0 private key in PEM or DER format. By default, the path is set to the currently used lab batch package, found in `../provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`. But it can be overriden by the user either from the command line when executing CMake (switch `-DLT_SH0_PRIV_PATH=<path>`), or from a child `CMakeLists.txt`.