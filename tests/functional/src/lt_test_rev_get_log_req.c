/**
 * @file lt_test_rev_get_log_req.c
 * @brief Tests Get_Log_Req command.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_test_common.h"

#define RANDOM_VALUE_GET_LOOPS 300

lt_handle_t *g_h;

static void lt_test_rev_get_log_req_body(uint32_t i_config_cfg_debug, uint32_t r_config_cfg_debug)
{
    uint8_t log_msg[TR01_GET_LOG_MAX_MSG_LEN + 1];  // +1 for '\0' added later
    uint16_t log_msg_read_size;
    int fw_log_en = (i_config_cfg_debug & BOOTLOADER_CO_CFG_DEBUG_FW_LOG_EN_MASK)
                    && (r_config_cfg_debug & BOOTLOADER_CO_CFG_DEBUG_FW_LOG_EN_MASK);

    LT_LOG_INFO("Getting RISC-V FW log...");
    LT_TEST_ASSERT_COND(lt_get_log_req(g_h, log_msg, TR01_GET_LOG_MAX_MSG_LEN, &log_msg_read_size), fw_log_en, LT_OK,
                        LT_L2_RESP_DISABLED);

    if (fw_log_en) {
        if (log_msg_read_size) {
            log_msg[log_msg_read_size] = '\0';
            LT_LOG_INFO("RISC-V FW log:");
            LT_LOG_INFO("%s", log_msg);
        }
        else {
            LT_LOG_INFO("Empty RISC-V FW log (log length is zero)");
        }
    }
    else {
        LT_LOG_INFO("RISC-V FW logging is disabled:");
        LT_LOG_INFO("I config FW_LOG_EN=%d", (int)i_config_cfg_debug & BOOTLOADER_CO_CFG_DEBUG_FW_LOG_EN_MASK);
        LT_LOG_INFO("R config FW_LOG_EN=%d", (int)r_config_cfg_debug & BOOTLOADER_CO_CFG_DEBUG_FW_LOG_EN_MASK);
    }
}

static lt_ret_t lt_test_rev_get_log_req_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Rebooting into Application mode...");
    ret = lt_reboot(g_h, TR01_REBOOT);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Couldn't reboot to the Application mode!");
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

void lt_test_rev_get_log_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_get_log_req()");
    LT_LOG_INFO("----------------------------------------------");

    g_h = h;

    uint32_t i_config_cfg_debug, r_config_cfg_debug;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Rebooting into Application mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_REBOOT));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(g_h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG_INFO("Reading CFG_DEBUG from I config...");
    LT_TEST_ASSERT(LT_OK, lt_i_config_read(g_h, TR01_CFG_DEBUG_ADDR, &i_config_cfg_debug));

    LT_LOG_INFO("Reading CFG_DEBUG from R config...");
    LT_TEST_ASSERT(LT_OK, lt_r_config_read(g_h, TR01_CFG_DEBUG_ADDR, &r_config_cfg_debug));

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(g_h));
    LT_LOG_LINE();

    lt_test_rev_get_log_req_body(i_config_cfg_debug, r_config_cfg_debug);
    LT_LOG_LINE();

    LT_LOG_INFO("Rebooting into Maintenance mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_MAINTENANCE_REBOOT));

    lt_test_cleanup_function = &lt_test_rev_get_log_req_cleanup;

    lt_test_rev_get_log_req_body(i_config_cfg_debug, r_config_cfg_debug);
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_get_log_req_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}