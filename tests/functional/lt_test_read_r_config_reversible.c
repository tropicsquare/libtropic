/**
 * @file lt_test_read_r_config_reversible.c
 * @brief Reads contents of R-Config and prints it to the log.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

void lt_test_read_r_config_reversible(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_read_r_config_reversible()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    struct lt_config_t r_config;

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Reading the whole R config:");
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config));
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08x", get_conf_desc(i), (unsigned int)r_config.obj[i]);
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(&h));
}
