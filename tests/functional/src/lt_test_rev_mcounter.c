/**
 * @file lt_test_rev_mcounter.c
 * @brief Test monotonic counter API - lt_mcounter_init, lt_mcounter_get, lt_mcounter_update.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"
#include "string.h"

// Shared with cleanup function.
lt_handle_t *g_h;

// The monotonic counter does not have any default value defined in the datasheet.
// Here we have chosen to initialize them to zero.
static lt_ret_t lt_test_rev_mcounter_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Starting secure session with slot %d.", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    for (int i = TR01_MCOUNTER_INDEX_0; i <= TR01_MCOUNTER_INDEX_15; i++) {
        LT_LOG_INFO("Initializing monotonic counter %d to zero...", i);
        ret = lt_mcounter_init(g_h, i, 0);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to set counter %d to zero.", i);
            return ret;
        }
    }

    LT_LOG_INFO("Aborting secure session");
    ret = lt_session_abort(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort secure session.");
        return ret;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle.");
        return ret;
    }

    return LT_OK;
}

void lt_test_rev_mcounter(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_mcounter()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    LT_LOG_INFO("Initializing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d.", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    lt_test_cleanup_function = &lt_test_rev_mcounter_cleanup;

    uint32_t init_val;
    uint32_t mcounter_val;
    const int max_decrements = 100;
    int current_decrements = 0;

    // Basic test: init to random value and try to decrement a few times.
    LT_LOG_INFO("Starting basic test...");
    for (int i = TR01_MCOUNTER_INDEX_0; i <= TR01_MCOUNTER_INDEX_15; i++) {
        LT_LOG_INFO("Generating random init value...");
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &init_val, sizeof(init_val)));
        LT_LOG_INFO("Initializing monotonic counter %d with %" PRIu32 "...", i, init_val);
        LT_TEST_ASSERT_COND(lt_mcounter_init(h, i, init_val), (init_val <= TR01_MCOUNTER_VALUE_MAX), LT_OK,
                            LT_PARAM_ERR);

        LT_LOG_INFO("Initializing monotonic counter %d again (should be ok)...", i);
        LT_TEST_ASSERT_COND(lt_mcounter_init(h, i, init_val), (init_val <= TR01_MCOUNTER_VALUE_MAX), LT_OK,
                            LT_PARAM_ERR);

        LT_LOG_INFO("Trying a few decrements...");
        current_decrements = 0;
        for (int expected_val = init_val; expected_val > 0 && current_decrements < max_decrements; expected_val--) {
            LT_LOG_INFO("Reading mcounter %d value...", i);
            LT_TEST_ASSERT(LT_OK, lt_mcounter_get(h, i, &mcounter_val));
            LT_LOG_INFO("Verifying mcounter value, should be: %d", expected_val);
            LT_TEST_ASSERT(expected_val, mcounter_val);

            LT_LOG_INFO("Decrementing...");
            LT_TEST_ASSERT(LT_OK, lt_mcounter_update(h, i));
            current_decrements++;
        }
    }
    LT_LOG_LINE();

    // Decrement to zero test: set to random small value and try to decrement to 0.
    LT_LOG_INFO("Starting decrement to zero test...");
    for (int i = TR01_MCOUNTER_INDEX_0; i <= TR01_MCOUNTER_INDEX_15; i++) {
        LT_LOG_INFO("Generating random small init value...");
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &init_val, sizeof(init_val)));
        init_val %= 100;
        LT_LOG_INFO("Initializing monotonic counter %d with %" PRIu32 "...", i, init_val);
        LT_TEST_ASSERT(LT_OK, lt_mcounter_init(h, i, init_val));

        LT_LOG_INFO("Decrementing to zero...");
        for (int expected_val = init_val; expected_val > 0; expected_val--) {
            LT_LOG_INFO("Reading mcounter %d value...", i);
            LT_TEST_ASSERT(LT_OK, lt_mcounter_get(h, i, &mcounter_val));
            LT_LOG_INFO("Verifying mcounter value, should be: %d", expected_val);
            LT_TEST_ASSERT(expected_val, mcounter_val);

            LT_LOG_INFO("Decrementing...");
            LT_TEST_ASSERT(LT_OK, lt_mcounter_update(h, i));
            current_decrements++;
        }

        LT_LOG_INFO("Try to decrement when mcounter is 0 (should fail)...");
        LT_TEST_ASSERT(LT_L3_UPDATE_ERR, lt_mcounter_update(h, i));
    }
    LT_LOG_LINE();

    // Assignment test: Assign each counter a known value and check
    // whether any counter was not overwritten. This will test
    // that there are no indexing problems.
    LT_LOG_INFO("Starting assignment test...");
    for (int i = TR01_MCOUNTER_INDEX_0; i <= TR01_MCOUNTER_INDEX_15; i++) {
        LT_LOG_INFO("Initializing monotonic counter %d with %d...", i, i);
        LT_TEST_ASSERT(LT_OK, lt_mcounter_init(h, i, i));
    }
    for (int i = TR01_MCOUNTER_INDEX_0; i <= TR01_MCOUNTER_INDEX_15; i++) {
        LT_LOG_INFO("Reading mcounter %d value...", i);
        LT_TEST_ASSERT(LT_OK, lt_mcounter_get(h, i, &mcounter_val));
        LT_LOG_INFO("Verifying mcounter value, should be: %d", i);
        LT_TEST_ASSERT(i, mcounter_val);
    }
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_mcounter_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}