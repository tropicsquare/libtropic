# TROPIC01 Model
The CMake project in the `tropic01_model/` directory builds libtropic so it can communicate with the TROPIC01 Python model. The CMake project offers the following:

1. Running libtropic's [Functional Tests](../../for_contributors/functional_tests.md). The testing is managed by CTest - it executes both the test and the model automatically, along with the creation of model configuration.
2. Running libtropic's [Examples](../../get_started/examples/index.md).
3. Flexible switching between the implemented CALs (Crypto Abstraction Layers) using the `LT_CAL` CMake variable.

> [!WARNING]
> Some examples are not compatible with the model because the model does not implement all of the chip's functionality. Those examples will always fail against the model and are therefore excluded in `tropic01_model/CMakeLists.txt`. Namely:
> 
> - `lt_ex_fw_update`,
> - `lt_ex_show_chip_id_and_fwver` (the model does not implement Bootloader mode, so you can use `tests/functional/lt_test_rev_get_info_req_app.c` to get this info from Application mode instead).

!!! note
    There is a symlink to a parent directory in the `tropic01_model` directory. This is required
    for coverage collection to work, as CMake does not include any files above the source directory
    in the coverage by default.

## How it Works?
The `tropic01_model/CMakeLists.txt` uses the TCP HAL implemented in `hal/posix/tcp/libtropic_port_posix_tcp.c`, so both processes (the compiled binary and the model) communicate through a TCP socket at 127.0.0.1:28992. The SPI layer between libtropic and the model is emulated through this TCP connection. The model responses match those of the physical TROPIC01 chip.

## Model Setup
First, install the model by following the README in the [ts-tvl](https://github.com/tropicsquare/ts-tvl) repository.

Next, you can initialize the model with data so it behaves like a real provisioned chip. To do that, pass a YAML configuration file to the model — see the [Model Server](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-server) and [Model Configuration](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration) sections in the [ts-tvl](https://github.com/tropicsquare/ts-tvl) repository. To create such a YAML configuration, use the `tropic01_model/create_model_cfg.py` script in the libtropic repository (example usage follows).

> [!IMPORTANT]
When running tests using CTest, no manual steps for creating the model configuration or initializing the model are necessary — CTest handles this. When running examples (or tests without CTest), start the model manually and apply a configuration so at least pairing key slot 0 is written to enable establishing a secure session.

Data, from which the `tropic01_model/create_model_cfg.py` script creates the YAML configuration file for the model, can be found in `tropic01_model/provisioning_data/` directory - see [Provisioning Data](provisioning_data.md) section for more information about the directory structure.

To create a model configuration that will initialize the model to the state which is almost identical to the provisioned chip, the `tropic01_model/create_model_cfg.py` script is run as:
```shell
cd tropic01_model/
python3 create_model_cfg.py --pkg-dir <path_to_the_lab_batch_package_directory>
```
where `<path_to_the_lab_batch_package_directory>` is the path to one of the lab batch packages inside `tropic01_model/provisioning_data/`. Running the script creates a file named `model_cfg.yml`, which can be passed directly to the model using the `-c` flag.

## Running the Examples
1. Switch to the `tropic01_model/` directory:
```shell
cd tropic01_model/
```
2. Compile the examples with the selected CAL (e.g. for MbedTLS v4.0.0):
```shell
mkdir build
cd build
cmake -DLT_BUILD_EXAMPLES=1 -DLT_CAL="mbedtls_v4" ..
make
```
As a result, executables for each example are built in the `tropic01_model/build/` directory.
> [!TIP]
To enable debugging symbols (e.g. to use [GDB](https://www.gnu.org/software/gdb/gdb.html)), add switch `-DCMAKE_BUILD_TYPE=Debug` when executing `cmake`.
>
> To use [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer) (ASan), add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_ASAN=1` when executing `cmake`.

3. Create a YAML configuration for the model from one of the lab batch packages:
```shell
python3 ../create_model_cfg.py --pkg-dir ../provisioning_data/2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/
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
It is recommended to run the tests using CTest, but if it's needed to run the tests under [GDB](https://www.gnu.org/software/gdb/gdb.html), they can be run exactly the same way as the examples.

1. Switch to the `tropic01_model/` directory:
```shell
cd tropic01_model/
```
2. Compile the tests with the selected CAL (e.g. for MbedTLS v4.0.0):
```shell
mkdir build
cd build
cmake -DLT_BUILD_TESTS=1 -DLT_CAL="mbedtls_v4" ..
make
```
As a result, executables for each test are built in the `tropic01_model/build/` directory.
> [!TIP]
To enable debugging symbols (e.g. to use [GDB](https://www.gnu.org/software/gdb/gdb.html)), add switch `-DCMAKE_BUILD_TYPE=Debug` when executing `cmake`.
>
> To use [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer) (ASan), add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_ASAN=1` when executing `cmake`.
>
> To execute the tests with [Valgrind](https://valgrind.org/), add switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_VALGRIND=1` when executing `cmake`. Note that [Valgrind](https://valgrind.org/) will be executed automatically only when using CTest!

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
The model is automatically started for each test separately, so it behaves like a fresh TROPIC01 straight out of factory. All this and other handling is done by the script `scripts/model_runner.py`, which is called by CTest.

??? warning "Problems with Secure Channel Session Due to Custom Model Configuration"
    Based on the TROPIC01 model configuration, you may encounter issues with tests or examples that establish a Secure Session. Examples and tests use production keys by default - see [Default Pairing Keys in Libtropic](../../get_started/default_pairing_keys.md#default-pairing-keys-in-libtropic) for more information.
    
    If you configured the model's pairing key slot 0 with engineering sample keys, you have to pass `-DLT_SH0_KEYS="eng_sample"` to `cmake` during the build.
    
    If you configured the model's pairing key slot 0 with some other keys, define the arrays for your private and public key as global and after `#include libtropic_examples.h` (or `#include libtropic_functional_tests.h`), do the following:
    ```c
    // Substitute LT_EX_SH0_PRIV for LT_TEST_SH0_PRIV in the case of test source file
    #undef LT_EX_SH0_PRIV
    #define LT_EX_SH0_PRIV <var_name_with_your_private_pairing_key>

    // Substitute LT_EX_SH0_PUB for LT_TEST_SH0_PUB in the case of test source file
    #undef LT_EX_SH0_PUB
    #define LT_EX_SH0_PUB <var_name_with_your_public_pairing_key>
    ```

### Running the Tests with Coverage
We support coverage collection for testing against the model. To activate coverage collection, add switch `-DLT_TEST_COVERAGE=1` when executing `cmake`, for example:
```shell
cmake -DLT_BUILD_TESTS=1 -DLT_TEST_COVERAGE=1 -DLT_CAL="mbedtls_v4" ..
```

After CTest finishes, you can use [gcovr](https://github.com/gcovr/gcovr) to export results:
```shell
# Execute this from the tropic01_model/ directory!
gcovr --txt coverage_report.txt --exclude 'build/_deps/.*|\.\./tests/.*|\.\./vendor/.*'
```

We use the following parameters:

- `--gcov-exclude` excludes selected files from report - we are not interested in measuring coverage of the tests themselves,
- `--txt` chooses text output format.

!!! tip
    You can use `--html` or `--html-details` output options to export in a HTML format or `--markdown` to export in a Markdown format.
    Check out [gcovr user guide](https://gcovr.com/en/latest/guide.html).