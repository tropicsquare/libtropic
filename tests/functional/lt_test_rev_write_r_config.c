/**
 * @file lt_test_rev_write_r_config.c
 * @brief Backs up R-Config, writes it and then restores it.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "string.h"

// Shared with cleanup function.
struct lt_config_t r_config_backup;
lt_handle_t h = {0};

lt_ret_t lt_test_rev_write_r_config_cleanup(void)
{
    if (LT_OK != verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0)) {
        return LT_FAIL;
    }
    if (LT_OK != write_whole_R_config(&h, &r_config_backup)) {
        return LT_FAIL;
    }
    if (LT_OK != lt_session_abort(&h)) {
        return LT_FAIL;
    }
    if (LT_OK != lt_deinit(&h)) {
        return LT_FAIL;
    }
}

void lt_test_rev_write_r_config(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_write_r_config()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    struct lt_config_t r_config_random, r_config;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Creating randomized R config for testing");
    LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(r_config_random.obj, LT_CONFIG_OBJ_CNT));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Backing up the whole R config:");
    LT_TEST_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_backup));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08x", cfg_desc_table[i].desc, (unsigned int)r_config_backup.obj[i]);
    }
    LT_LOG_LINE();

    // No we have the R config backed up. From this moment now it makes
    // sense to call the cleanup function.
    lt_test_cleanup_function = &lt_test_rev_write_r_config_cleanup;

    LT_LOG_INFO("Writing the whole R config");
    LT_TEST_ASSERT(LT_OK, write_whole_R_config(&h, &r_config_random));

    LT_LOG_INFO("Reading the whole R config");
    LT_TEST_ASSERT(LT_OK, read_whole_R_config(&h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08x", cfg_desc_table[i].desc, (unsigned int)r_config.obj[i]);
        LT_LOG_INFO("Checking if it was written");
        LT_TEST_ASSERT(1, (r_config.obj[i] == r_config_random.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Writing the whole R config again (should fail)");
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_TEST_ASSERT(LT_L3_FAIL, lt_r_config_write(&h, cfg_desc_table[i].addr, r_config_backup.obj[i]));
    }

    LT_LOG_INFO("Reading the whole R config");
    memset(r_config.obj, 0, sizeof(r_config.obj));
    LT_TEST_ASSERT(LT_OK, read_whole_R_config(&h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08x", cfg_desc_table[i].desc, (unsigned int)r_config.obj[i]);
        LT_LOG_INFO("Checking if it was not rewritten");
        LT_TEST_ASSERT(1, (r_config.obj[i] == r_config_random.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Erasing R config before restoring (needed to be able to write again)");
    LT_TEST_ASSERT(LT_OK, lt_r_config_erase(&h));

    LT_LOG_INFO("Restoring the whole R config");
    LT_TEST_ASSERT(LT_OK, write_whole_R_config(&h, &r_config_backup));

    LT_LOG_INFO("Reading the whole R config");
    LT_TEST_ASSERT(LT_OK, read_whole_R_config(&h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08x", cfg_desc_table[i].desc, (unsigned int)r_config.obj[i]);
        LT_LOG_INFO("Checking if it was restored");
        LT_TEST_ASSERT(1, (r_config_backup.obj[i] == r_config.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Starting post-test cleanup.");
    if (LT_OK != lt_test_rev_write_r_config_cleanup()) {
        LT_LOG_ERROR("Cleanup failed!");
    } else {
        LT_LOG_INFO("Cleanup OK!");
    }
}