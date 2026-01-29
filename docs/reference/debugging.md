# Debugging
When debugging, some additional compiler flags are needed to produce debugging information. These flags can be enabled using CMake's [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) option. This option can be set similarly as the Libtropic's CMake options — see [How to Configure](integrating_libtropic/how_to_configure/index.md) section.

After this, you can use debugging tools of your choice, e.g. [GNU Debugger](https://www.gnu.org/savannah-checkouts/gnu/gdb/index.html) or [Valgrind](https://valgrind.org/). Refer to each tool's documentation for installation instructions.

## Debugging the Tests

In our [functional](../for_contributors/tests/functional_tests.md) and [functional mock](../for_contributors/tests/functional_mock_tests.md) tests we support running with both [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer) and [Valgrind](https://valgrind.org/) on Linux (USB, SPI and model HALs):

- To use [AddressSanitizer](https://github.com/google/sanitizers/wiki/addresssanitizer): add CMake switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_ASAN=1`.
- To use [Valgrind](https://valgrind.org/): add CMake switches `-DCMAKE_BUILD_TYPE=Debug` and `-DLT_VALGRIND=1`.
    
You can also use GNU Debugger to debug the tests:

- Add CMake switch `-DCMAKE_BUILD_TYPE=Debug` and run the binary using `gdb`.
    - If you want to use the debugger with the model, you can't use the model runner. Run the model server manually instead and then run the test binary using `gdb`. See the [model tutorial](../tutorials/model/index.md).

!!! question "Not sure if ASan or Valgrind is working?"
    Paste one of the following snippets to a source file where you want to check the error detection.

    ```c { .copy }
    char *p = malloc(10);
    if (!p) return 1;
    p[10] = 'X'; // one-past-end heap buffer write -> ASan/Valgrind should report
    free(p);
    ```

    ```c { .copy }
    char *p = malloc(16);
    if (!p) return 1;
    free(p);
    p[0] = 'X'; // use-after-free -> ASan/Valgrind should report
    ```