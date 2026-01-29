# Code Coverage
We use two groups of tests: functional (which use the standard HAL) and functional mock (which use the mock HAL only).

To measure total code coverage from both functional and functional mock tests, combine the coverage data. First, run tests with coverage for each group separately using the guides below.

!!! note "Implementation remarks"
    - We do not measure HAL coverage currently, because not all platforms support coverage collection.
    - CAL is also excluded from coverage collection because we cannot mock CFP return values, which makes full coverage impossible without unit tests.

## Running Functional Tests with Coverage against Model
We support collecting coverage only against [TROPIC01 Model](../../tutorials/model/index.md) (refer to this link for preparation instructions).

!!! example "Collecting coverage of Functional Tests"
    Create a `build/` directory and switch to it:
    ```bash { .copy }
    cd tests/functional/model/
    mkdir -p build/
    cd build/
    ```
    To activate coverage collection, add the switch `-DLT_TEST_COVERAGE=1` when invoking `cmake`:
    ```bash { .copy }
    cmake -DLT_TEST_COVERAGE=1 -DLT_CAL="mbedtls_v4" ..
    make
    ctest -V
    ```

    After CTest finishes, use [gcovr](https://github.com/gcovr/gcovr) to export results (execute from the `tests/functional/model/build/` directory):
    ```bash { .copy }
    gcovr --json coverage_trace.json --exclude '.*_deps/.*|.*cal/.*|.*hal/.*' -r ../../../..
    ```

## Running Functional Mock Tests
Run functional mock tests on the same platform you used for the previous tests. The tests can be compiled and run as following:

!!! example "Collecting coverage of Functional Mock Tests"
    Create a `build/` directory and switch to it:
    ```bash { .copy }
    cd tests/functional_mock/
    mkdir -p build/
    cd build/
    ```
    To activate coverage collection, add the switch `-DLT_TEST_COVERAGE=1` when invoking `cmake`:
    ```bash { .copy }
    cmake -DLT_TEST_COVERAGE=1 ..
    make
    ctest -V
    ```

    After CTest finishes, use [gcovr](https://github.com/gcovr/gcovr) to export results (execute from the `tests/functional_mock/build/` directory):
    ```bash { .copy }
    gcovr --json coverage_trace.json --exclude '.*_deps/.*|.*cal/.*|.*hal/.*' -r ../../..
    ```

## Merging and Exporting Total Coverage
Merge results and export in text format:
!!! example "Merging Coverage Results and Exporting to Text Format"
    Execute this from the repository root (or adjust paths accordingly):
    ```bash { .copy }
    gcovr --json-add-tracefile tests/functional_mock/build/coverage_trace.json \
          --json-add-tracefile tests/functional/model/build/coverage_trace.json \
          --txt coverage.txt
    ```

!!! tip "Tip: Gcovr Output Formats"
    You can use `--html` or `--html-details` output options to export in a HTML format or `--markdown` to export in a Markdown format instead of `--txt`.
    Check out [gcovr user guide](https://gcovr.com/en/latest/guide.html).
