# Functional Tests
Functional tests are used to verify the Libtropic core API and are implemented in `tests/functional/`.  Running functional tests is supported on all platforms, although not all tests are compatible with each platform (see [Compatibility](#compatibility)). There are CI jobs in Libtropic repository which run the tests against TROPIC01 Model with ASan and Valgrind.

The functional tests are organized into two categories, as some of them may cause irreversible changes to the chip:

- **Reversible** (`lt_test_rev_*.c`): only reversible operations are executed on the TROPIC01.
- **Irreversible** (`lt_test_ire_*.c`): irreversible operations are executed: the state or contents of the TROPIC01 **cannot** be reverted.

## Compilation and Running
!!! danger "DANGER!"
    Functional tests are for internal use only and are provided only for reference. Some tests can **destroy** your chip. **Do not run the tests** unless you use model only or you are absolutely sure what you are doing. If you damage your chip with the tests, we are unable to provide any support.

For each supported host platform (HAL), there is a subdirectory in `tests/functional`. Tests can be compiled as following (we will use model as an example):

```bash
cd tests/functional/model
mkdir build
cd build
cmake -DLT_CAL=mbedtls_v4 ..
make
```

As you can see, there is one mandatory parameter for selection of the Crypto Abstraction Layer. See `cal/` directory for currently supported CALs or just run `cmake -DLT_CAL= ..` for enumeration of supported options. There are also other options, see [Available Options](#available-options).

After compilation is finished, the tests can be run using CTest. To see available tests, run:

```shell
ctest -N
```
To select specific test(s) using regular expression, run:
```shell
ctest -R <test_regex>
```
where `<test_regex>` is a regular expression for the test names from the list.

To run all reversible tests, simply run:
```shell
ctest _rev_
```
  
To exclude some tests, run:
```shell
ctest -E <test_regex>
```
where `<test_regex>` is a regular expression for the test names from the list.

### Test Logs
Tests on the model output all logging information into separate files in `build/run_logs`. All other platforms output logging to stdout, which can be redirected either using standard [Bash redirection](https://www.gnu.org/software/bash/manual/html_node/Redirections.html) or using [CTest options](https://cmake.org/cmake/help/latest/manual/ctest.1.html#run-tests).

To enable verbose output from CTest, run `ctest -V` or `ctest -W` switch for even more verbose output.

### Available Options

Options common for all host platforms:

| Option                  | Description                                                                 | Type    | Default |
|-------------------------|-----------------------------------------------------------------------------|---------|---------|
| `LT_ASAN`               | Enables static AddressSanitizer                                             | boolean | OFF     |
| `LT_CAL`                | Flexible switching between the implemented CALs (Crypto Abstraction Layers) | string  | -       |
| `LT_STRICT_COMPILATION` | Enables strict compilation flags                                            | boolean | ON      |
| `LT_VALGRIND`           | CTest runs the binaries with Valgrind                                       | boolean | OFF     |

### Debugging
We support running tests with GNU Debugger, Valgrind and Address Sanitizer. See [Debugging](../../reference/debugging.md).

## Compatibility
Not all tests are compatible with the model, as it does not implement all of the TROPIC01's functionality. To see which tests are unavailable for the model, simply run `ctest -N` and compare with the sources or have a look at `CMakeLists.txt`, where the tests are removed using `list(REMOVE_ITEM LIBTROPIC_TEST_LIST <list>)`.

## Adding a New Test
To add a new test, you need to:

1. Decide whether the test is reversible or irreversible (see [Test Types and Cleanup](#test-types-and-cleanup) if you are not sure).
2. Write the new test (see [Test Template](#test-template)).
3. Add the declaration together with a Doxygen comment to `include/libtropic_functional_tests.h`.
4. Add the test to the root `CMakeLists.txt`:
        - In the section "LIBTROPIC FUNCTIONAL TESTS", add the test name to the `LIBTROPIC_TEST_LIST` (it must match the name of the function that implements the test)
        - Below the `LIBTROPIC_TEST_LIST`, there is a section where `SDK_SRCS` is extended
            with test source files. Add your test source file there.
5. Make sure your test works - you can run it against the model. If the test
   fails, you either:
    - Did a mistake in the test. Fix it.
    - Or you found a bug - if you are certain it is a bug and not a problem in your test,
      [open an issue](https://github.com/tropicsquare/libtropic/issues/new). Thanks!

### Test Types and Cleanup
As the tests are also ran against the real chips, we recognize two types of tests:

1. *Reversible*. This type of test shall not make any irreversible changes to the chip. It may
   happen that the test is interrupted by a failed assert. For these cases, there is a possibility
   to define a cleanup function, which is called on every failed assert before the test termination.
   If the test does some changes to the chip, which should be reverted after the end of the test,
   the cleanup function for the test **must** be implemented, to make the test truly reversible.
2. *Irreversible*. This type of test causes changes that are not reversible by nature (e.g., I-Config
    modifications). These tests do not have to implement a cleanup function, since the chip state or
    contents cannot be reverted after the test ends.

#### Cleanup Function
If an assert fails, the assert function checks whether the `lt_test_cleanup_function` function pointer
is not `NULL`. If so, the cleanup function is called automatically before terminating the test. By default, the pointer is initialized to `NULL`.

If you need a cleanup function, create the function and assign it to `lt_test_cleanup_function` at the appropriate point in the test (for example, after you back up data that you will restore later).

to duplicate the cleanup code if it would be the same. If you wrap the function call in the `LT_TEST_ASSERT`, do not forget to set `lt_test_cleanup_function` back to `NULL` beforehands, otherwise the cleanup will be called twice.
You can reuse your cleanup function at the end of the test so you don't have to duplicate the cleanup code. If you wrap the function call in `LT_TEST_ASSERT`, remember to set `lt_test_cleanup_function` back to `NULL` beforehand, otherwise the cleanup will be called twice.

### Test Template
Change the lines marked with `TODO`.

```c
/**
 * @file TODO: FILL ME
 * @brief TODO: FILL ME
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see the LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

// Shared with cleanup function.
// TODO: CAN BE REMOVED IF CLEANUP IS NOT USED.
lt_handle_t *g_h;

// TODO: REMOVE OR EDIT
static lt_ret_t lt_new_test_cleanup(void)
{
    LT_LOG_INFO("Starting secure session with slot %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session, ret=%s", lt_ret_verbose(ret));
        return ret;
    }

    // TODO: REST OF THE CLEANUP DUTIES

    return LT_OK;
}

void lt_new_test(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_new_test()");
    LT_LOG_INFO("----------------------------------------------");

    // TODO/NOTE: If you use cleanup, do not forget to assign handle to a globally available pointer,
    // so you can use the handle in the cleanup function.
    g_h = h;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting secure session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // TODO: DO THE TESTING HERE
    // Add this line if you need cleanup. Add it where appropriate -- e.g. after backing up data
    // you need to restore in the cleanup.
    lt_test_cleanup_function = &lt_new_test_cleanup;

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_new_test_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}
```

## Advanced and Troubleshooting

### Running a Test With Your Own Pairing Key
If you have already written your own public key to one of the available slots and want to execute a test that uses a Secure Session, define the arrays for your private and public key as globals and, after `#include "lt_test_common.h"`, do the following:

```c
#undef LT_TEST_SH0_PRIV
#define LT_TEST_SH0_PRIV <var_name_with_your_private_pairing_key>

#undef LT_TEST_SH0_PUB
#define LT_TEST_SH0_PUB <var_name_with_your_public_pairing_key>
```

### Cannot Establish a Secure Session
Refer to a dedicated section in the [FAQ](../../faq.md#i-cannot-establish-a-secure-session).