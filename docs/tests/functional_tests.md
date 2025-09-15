# Functional Tests
Functional tests are implemented in `tests/functional_tests/`.

Some tests may cause irreversible changes to the chip, so they are organized into two categories:

- **reversible** (`lt_test_rev_*.c`): only reversible operations are executed on the TROPIC01 chip,
- **irreversible** (`lt_test_ire_*.c`): irreversible operations are executed, so the TROPIC01 chip may get bricked.
!!! note
    Functional tests are not compiled by default. To compile functional tests, either

    - pass `-DLT_BUILD_TESTS=1` to `cmake` during compilation, or
    - in your CMake file, switch the option on: `set(LT_BUILD_TESTS ON)`.

!!! note
    During build, SH0 keypair is automatically chosen from `libtropic/provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`, this SH0 key is present in the majority of distributed TROPIC01 chips. In certain cases (first   engineering samples) it might be necessary to manually set it (in PEM or DER format) with following cmake switch: `-DLT_SH0_PRIV_PATH=<path to sh0_priv_engineering_sample01.pem>`

## Adding a New Test
To add a new test, you need to:

1. Decide whether the test is reversible or not. See [below](#test-types-and-cleanup).
2. Write the new test. Use the [template below](#test-template).
3. Add the declaration together with a Doxygen comment to `include/libtropic_functional_tests.h`.
4. Add the test to the root `CMakeLists.txt`:
    - In the section "LIBTROPIC FUNCTIONAL TESTS", add the test name to the `LIBTROPIC_TEST_LIST`
      (it has to be the same as the name of the function which implements the test)
    - Below the `LIBTROPIC_TEST_LIST`, there is a section where `SDK_SRCS` is extended
      with test source files. Add source file of your test here.
5. Make sure your test works. Use the model: check out [TROPIC01 Model](../other/tropic01_model.md) section. If the test
   fails, you either:
    - Did a mistake in the test. Fix it.
    - Or you found a bug -- if you are certain it is a bug and not a problem in your test,
      [open an issue](https://github.com/tropicsquare/libtropic/issues/new). Thanks!

### Test Types and Cleanup
As the tests are also ran against real chips, we recognize two types of tests:

1. Reversible -- this type of test shall not make any irreversible changes to the chip. It may
   happen that the test is interrupted by a failed assert. For this cases there is a possibility
   to define a cleanup function, which is called on every failed assert before test termination.
   Every test that do some changes which require cleanup afterwards for the test to be truly reversible
   MUST contain the cleanup function.
2. Irreversible -- this type of test causes changes which are not reversible by nature (e.g. I-config
   modifications). Those test do not have to implement cleanup function, as the chip is always
   "destroyed" after running the test.

#### Cleanup Function
If the assert fails, the assert function checks whether the `lt_test_cleanup_function` function pointer
is not `NULL`. If not, the cleanup function is called automatically before terminating the test. By default, the pointer is initialized to `NULL`.

If you need cleanup function, please create the function and assign the `lt_test_cleanup_function`
at the right moment in the test (e.g. after you backed up data you would like to restore).

You can of course reuse your cleanup function at the end of the test, so you don't have
to duplicate the cleanup code if it would be the same. If you wrap the function call in the `LT_TEST_ASSERT`, do not forget to set `lt_test_cleanup_function` back to `NULL` beforehands, otherwise the cleanup will be called twice.

### Test Template
Change the lines with `TODO`.

```c
/**
 * @file TODO: FILL ME
 * @brief TODO: FILL ME
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

// Shared with cleanup function.
// TODO: CAN BE REMOVED IF CLEANUP NOT USED.
lt_handle_t *g_h;

// TODO: REMOVE OR EDIT
lt_ret_t lt_new_test_cleanup(void)
{
    LT_LOG_INFO("Starting secure session with slot %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
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

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
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