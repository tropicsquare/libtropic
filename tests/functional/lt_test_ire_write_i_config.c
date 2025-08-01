/**
 * @file lt_test_ire_write_i_config.c
 * @brief Backs up R-Config, writes it and then restores it.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "string.h"

void lt_test_ire_write_i_config(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_ire_write_i_config()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    struct lt_config_t i_config_random, i_config;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Creating randomized I config for testing");
    LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(i_config_random.obj, LT_CONFIG_OBJ_CNT));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Writing the whole I config");
    LT_TEST_ASSERT(LT_OK, write_whole_I_config(&h, &i_config_random));
    LT_LOG_LINE();

    LT_LOG_INFO("Reading the whole I config");
    LT_TEST_ASSERT(LT_OK, read_whole_I_config(&h, &i_config));
    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, i_config.obj[i]);
        LT_LOG_INFO("Checking if it was written");
        LT_TEST_ASSERT(1, (i_config.obj[i] == i_config_random.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(&h));
}