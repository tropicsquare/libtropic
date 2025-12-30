/**
 * @file lt_test_rev_startup_req.c
 * @brief Test L2 Startup Request.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_l2.h"
#include "libtropic_logging.h"
#include "lt_l2_api_structs.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"

#define REBOOT_WAIT_ATTEMPTS 10

lt_handle_t *g_h;
enum lt_current_chip_state { MAINTENANCE_MODE, APPLICATION_MODE, CHIP_BUSY };

static enum lt_current_chip_state check_current_state(void)
{
    uint8_t spect_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE];
    lt_ret_t ret;

    LT_LOG_INFO("Retrieving SPECT FW version...");
    for (int i = 0; i < REBOOT_WAIT_ATTEMPTS; i++) {
        ret = lt_get_info_spect_fw_ver(g_h, spect_ver);
        if (LT_OK == ret) {
            break;
        }
        else if (LT_L1_CHIP_BUSY == ret) {
            LT_LOG_INFO("Chip busy, waiting and trying again...");
            LT_TEST_ASSERT(LT_OK, lt_l1_delay(&g_h->l2, LT_TR01_REBOOT_DELAY_MS));
        }
    }

    if (LT_OK == ret) {
        LT_LOG_INFO("OK!");
    }
    else {
        LT_LOG_ERROR("Chip still busy! Terminating test.");
        return CHIP_BUSY;
    }

    LT_LOG_INFO("Spect version:");
    hexdump_8byte(spect_ver, TR01_L2_GET_INFO_SPECT_FW_SIZE);
    if (0 == memcmp(spect_ver, "\x00\x00\x00\x80", TR01_L2_GET_INFO_SPECT_FW_SIZE)) {
        return MAINTENANCE_MODE;
    }
    else {
        return APPLICATION_MODE;
    }
}

static lt_ret_t lt_test_rev_startup_req_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Rebooting to the app mode...");
    ret = lt_reboot(g_h, TR01_REBOOT);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Couldn't reboot to the app mode!");
        return ret;
    }

    LT_LOG_INFO("Checking the chip is in the app mode...");
    if (APPLICATION_MODE != check_current_state()) {
        LT_LOG_ERROR("Still in maintenance mode!");
        return LT_FAIL;
    }

    return LT_OK;
}

void lt_test_rev_startup_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------");
    LT_LOG_INFO("lt_test_rev_startup_req()");
    LT_LOG_INFO("----------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    lt_test_cleanup_function = &lt_test_rev_startup_req_cleanup;

    // Part 1: Try to reboot from app mode to app mode twice. First time to make sure we are in
    // the app mode, second time to try rebooting to the app mode from the app mode.
    for (int i = 0; i < 2; i++) {
        LT_LOG_INFO("Rebooting to the app mode...");
        LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_REBOOT));
        LT_LOG_INFO("Checking the chip is in the normal mode...");
        LT_TEST_ASSERT(APPLICATION_MODE, check_current_state());
    }

    // Part 2: Try to reboot from app to maintenance mode and from maintenance to maintenance mode.
    for (int i = 0; i < 2; i++) {
        LT_LOG_INFO("Rebooting to the maintenance mode (maintenance reboot)...");
        LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_MAINTENANCE_REBOOT));
        LT_LOG_INFO("Checking the chip is in the maintenance mode...");
        LT_TEST_ASSERT(MAINTENANCE_MODE, check_current_state());
        LT_LOG_INFO("Checking that the handshake does not work...");
#ifdef ABAB
        LT_TEST_ASSERT(LT_L2_GEN_ERR, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                              TR01_PAIRING_KEY_SLOT_INDEX_0));

#elif ACAB
        LT_TEST_ASSERT(LT_L2_UNKNOWN_REQ, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
#else
#error "Undefined silicon revision. Please define either ABAB or ACAB."
#endif
    }

    // Part 3: Try to reboot from maintenance mode to app mode.
    LT_LOG_INFO("Rebooting to the app mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_REBOOT));
    LT_LOG_INFO("Checking the chip is in the app mode...");
    LT_TEST_ASSERT(APPLICATION_MODE, check_current_state());
}