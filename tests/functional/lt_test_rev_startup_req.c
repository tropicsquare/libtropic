/**
 * @file lt_test_rev_startup_req.c
 * @brief Test L2 Startup Request.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_l1_port_wrap.h"
#include "lt_l2.h"
#include "lt_l2_api_structs.h"

#define REBOOT_WAIT_ATTEMPTS 10

lt_handle_t *lt_h;
enum lt_tropic01_mode { LT_BOOTLOADER_MODE, LT_NORMAL_MODE, LT_BUSY };

enum lt_tropic01_mode check_current_mode(void)
{
    uint8_t spect_ver[LT_L2_GET_INFO_SPECT_FW_SIZE];
    lt_ret_t ret;

    LT_LOG_INFO("Retrieving SPECT FW version...");
    for (int i = 0; i < REBOOT_WAIT_ATTEMPTS; i++) {
        ret = lt_get_info_spect_fw_ver(lt_h, spect_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
        if (LT_OK == ret) {
            break;
        }
        else if (LT_L1_CHIP_BUSY == ret) {
            LT_LOG_INFO("Chip busy, waiting and trying again...");
            lt_l1_delay(&lt_h->l2, LT_TROPIC01_REBOOT_DELAY_MS);
        }
    }

    if (LT_OK == ret) {
        LT_LOG_INFO("OK!");
    }
    else {
        LT_LOG_ERROR("Chip still busy! Terminating test.");
        return LT_BUSY;
    }

    LT_LOG_INFO("Spect version:");
    hexdump_8byte(spect_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
    if (0 == memcmp(spect_ver, "\x00\x00\x00\x80", LT_L2_GET_INFO_SPECT_FW_SIZE)) {
        return LT_BOOTLOADER_MODE;
    }
    else {
        return LT_NORMAL_MODE;
    }
}

lt_ret_t lt_test_rev_startup_req_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Rebooting to the normal mode...");
    ret = lt_reboot(lt_h, LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Couldn't reboot to the normal mode!");
        return ret;
    }

    LT_LOG_INFO("Checking we are again in the normal mode...");
    if (LT_NORMAL_MODE != check_current_mode()) {
        LT_LOG_ERROR("Still in bootloader mode!");
        return LT_FAIL;
    }

    return LT_OK;
}

void lt_test_rev_startup_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------");
    LT_LOG_INFO("lt_test_rev_startup_req()");
    LT_LOG_INFO("----------------------------------");

    lt_h = h;

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    lt_test_cleanup_function = &lt_test_rev_startup_req_cleanup;

    // Part 1: Try to reboot from normal to normal.
    LT_LOG_INFO("Checking we are in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode());
    LT_LOG_INFO("Rebooting to the normal mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT));
    LT_LOG_INFO("Checking we are again in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode());

    // Part 2: Try to reboot from normal to bootloader and from bootloader to bootloader.
    for (int i = 0; i < 2; i++) {
        LT_LOG_INFO("Rebooting to the bootloader mode (maintenance reboot)...");
        LT_TEST_ASSERT(LT_OK, lt_reboot(h, LT_L2_STARTUP_REQ_STARTUP_ID_MAINTENANCE_REBOOT));
        LT_LOG_INFO("Checking we are in the bootloader mode...");
        LT_TEST_ASSERT(LT_BOOTLOADER_MODE, check_current_mode());
        LT_LOG_INFO("Checking that the handshake does not work...");
        LT_TEST_ASSERT(LT_L2_UNKNOWN_REQ,
                       verify_chip_and_start_secure_session(h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    }

    // Part 3: Try to reboot from bootloader to normal.
    LT_LOG_INFO("Rebooting to the normal mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT));
    LT_LOG_INFO("Checking we are again in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode());
}