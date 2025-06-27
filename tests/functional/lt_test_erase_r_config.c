/**
 * @file lt_test_erase_r_config.c
 * @brief Test function which erases R config
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

/**
 * @brief
 *
 * @return int
 */
int lt_test_erase_r_config(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_erase_r_config() "
        "-------------------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));
    LT_LOG("%s with SH%d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("lt_r_config_erase()");
    LT_ASSERT(LT_OK, lt_r_config_erase(&h));

    LT_LOG("%s", "R CONFIG read:");
    struct lt_config_t r_config_read;
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i = 0; i < 27; i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("%s", "lt_reboot(LT_MODE_APP)");
    LT_ASSERT(LT_OK, lt_reboot(&h, LT_MODE_APP));

    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
