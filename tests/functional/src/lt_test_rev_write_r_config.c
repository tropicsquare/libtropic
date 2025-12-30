/**
 * @file lt_test_rev_write_r_config.c
 * @brief Backs up R-Config, writes it and then restores it.
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
struct lt_config_t r_config_backup;
lt_handle_t *g_h;

static lt_ret_t lt_test_rev_write_r_config_cleanup(void)
{
    lt_ret_t ret;
    struct lt_config_t r_config;

    LT_LOG_INFO("Starting secure session with slot %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    LT_LOG_INFO("Erasing R config, so it can be restored");
    ret = lt_r_config_erase(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to erase R config.");
        return ret;
    }

    LT_LOG_INFO("Writing R config backup");
    ret = lt_write_whole_R_config(g_h, &r_config_backup);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to write R config.");
        return ret;
    }

    LT_LOG_INFO("Reading R config and checking if restored correctly");
    ret = lt_read_whole_R_config(g_h, &r_config);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to read R config.");
        return ret;
    }
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        if (r_config.obj[i] != r_config_backup.obj[i]) {
            LT_LOG_ERROR("Slot %d was not correctly restored", i);
            return LT_FAIL;
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

void lt_test_rev_write_r_config(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_write_r_config()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    struct lt_config_t r_config_random, r_config;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Creating randomized R config for testing");
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, r_config_random.obj, sizeof(r_config_random.obj)));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Backing up the whole R config:");
    LT_TEST_ASSERT(LT_OK, lt_read_whole_R_config(h, &r_config_backup));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, r_config_backup.obj[i]);
    }
    LT_LOG_LINE();

    // No we have the R config backed up. From this moment now it makes
    // sense to call the cleanup function.
    lt_test_cleanup_function = &lt_test_rev_write_r_config_cleanup;

    LT_LOG_INFO("Erasing R config, so it can be written.");
    LT_ASSERT(LT_OK, lt_r_config_erase(h));

    LT_LOG_INFO("Writing the whole R config");
    LT_TEST_ASSERT(LT_OK, lt_write_whole_R_config(h, &r_config_random));

    LT_LOG_INFO("Reading the whole R config");
    LT_TEST_ASSERT(LT_OK, lt_read_whole_R_config(h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, r_config.obj[i]);
        LT_LOG_INFO("Checking if it was written");
        LT_TEST_ASSERT(1, (r_config.obj[i] == r_config_random.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Writing the whole R config again (should fail)");
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_TEST_ASSERT(LT_L3_FAIL, lt_r_config_write(h, cfg_desc_table[i].addr, r_config_backup.obj[i]));
    }

    LT_LOG_INFO("Reading the whole R config");
    memset(r_config.obj, 0, sizeof(r_config.obj));
    LT_TEST_ASSERT(LT_OK, lt_read_whole_R_config(h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, r_config.obj[i]);
        LT_LOG_INFO("Checking if it was not rewritten");
        LT_TEST_ASSERT(1, (r_config.obj[i] == r_config_random.obj[i]));
    }
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_write_r_config_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}