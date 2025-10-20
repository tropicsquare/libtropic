# Functional Tests
Functional tests are used to verify the libtropic core API and are implemented in `tests/functional/`. In the `libtropic` repository, these tests are run against the [TROPIC01 Model](../other/tropic01_model/index.md) only. Testing aganist the TROPIC01 model is also utilized in a CI job, triggered for the `master` and `develop` branches (pushes and pull requests). The tests can also be run in the [libtropic platform repositories](https://github.com/tropicsquare/libtropic#get-started).

The functional tests are organized into two categories, as some of them may cause irreversible changes to the chip:

- **Reversible** (`lt_test_rev_*.c`): only reversible operations are executed on the TROPIC01 chip.
- **Irreversible** (`lt_test_ire_*.c`): irreversible operations are executed - the state or contents of the TROPIC01 chip **cannot** be reverted.
!!! note
    Functional tests are not compiled by default. To compile functional tests, either

    - pass `-DLT_BUILD_TESTS=1` to `cmake` during compilation, or
    - in your CMake file, switch the option on: `set(LT_BUILD_TESTS ON)`.

!!! warning
    You may encounter issues with tests that establish a Secure Session. Although the default (production) key is present in the majority of distributed TROPIC01 chips, the first chip revisions (engineering samples) might contain a different key. To successfully establish a Secure Session with one of the engineering samples, pass `-DLT_SH0_KEYS="eng_sample"` to `cmake` during the build. In the case of the production chips, no additional actions are needed (the correct key is set by default).

??? tip "Tip: Running a Test with Custom Pairing Key"
    If you want to execute one of the tests (that uses a Secure Session) with your custom pairing key, define the arrays for private and public key as global and after `#include libtropic_examples.h`, do the following:
    ```c
    #undef LT_TEST_SH0_PRIV
    #define LT_TEST_SH0_PRIV <var_name_with_your_private_pairing_key>

    #undef LT_TEST_SH0_PUB
    #define LT_TEST_SH0_PUB <var_name_with_your_public_pairing_key>
    ```

## Adding a New Test
To add a new test, you need to:

1. Decide whether the test is reversible or not (see [Test Types and Cleanup](#test-types-and-cleanup) if you are not sure).
2. Write the new test (see [Test Template](#test-template)).
3. Add the declaration together with a Doxygen comment to `include/libtropic_functional_tests.h`.
4. Add the test to the root `CMakeLists.txt`:
    - In the section "LIBTROPIC FUNCTIONAL TESTS", add the test name to the `LIBTROPIC_TEST_LIST`
      (it has to be the same as the name of the function which implements the test)
    - Below the `LIBTROPIC_TEST_LIST`, there is a section where `SDK_SRCS` is extended
      with test source files. Add the source file of your test here.
5. Make sure your test works - you can run it against the [TROPIC01 Model](../other/tropic01_model/index.md). If the test
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
2. *Irreversible*. This type of test causes changes which are not reversible by nature (e.g. I-Config
   modifications). These tests do not have to implement the cleanup function, as the chip state or
   contents cannot be reverted after the test ends.

#### Cleanup Function
If the assert fails, the assert function checks whether the `lt_test_cleanup_function` function pointer
is not `NULL`. If not, the cleanup function is called automatically before terminating the test. By default, the pointer is initialized to `NULL`.

If you need a cleanup function, please create the function and assign the `lt_test_cleanup_function`
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

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
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