/**
 * @file lt_test_ire_write_i_config.c
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

void lt_test_ire_write_i_config(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_ire_write_i_config()");
    LT_LOG_INFO("----------------------------------------------");

    struct lt_config_t i_config_random, i_config;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Creating randomized I config for testing");
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, i_config_random.obj, sizeof(i_config_random.obj)));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Writing the whole I config");
    LT_TEST_ASSERT(LT_OK, lt_write_whole_I_config(h, &i_config_random));
    LT_LOG_LINE();

    LT_LOG_INFO("Reading the whole I config");
    LT_TEST_ASSERT(LT_OK, lt_read_whole_I_config(h, &i_config));
    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, i_config.obj[i]);
        LT_LOG_INFO("Checking if it was written");
        LT_TEST_ASSERT(1, (i_config.obj[i] == i_config_random.obj[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}