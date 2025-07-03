/**
 * @file lt_test_dump_r_config.c
 * @brief Dumps content of Tropic01 R-Config to the log.
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
int lt_test_dump_r_config(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_dump_r_config() "
        "----------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};
    struct lt_config_t r_config;

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("%s", "read_whole_R_config");
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config));

    LT_LOG("%s", "R Config");
    for (int i = 0; i < (sizeof(struct lt_config_t) / sizeof(uint32_t)); i++) {
        LT_LOG("%d: 0x%08x", i * 4, (unsigned int)r_config.obj[i]);
    }

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
