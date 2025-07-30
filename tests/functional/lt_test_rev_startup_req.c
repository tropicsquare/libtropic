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
#include "lt_l2.h"
#include "lt_l2_api_structs.h"

enum lt_tropic01_mode { LT_BOOTLOADER_MODE, LT_NORMAL_MODE };
enum lt_tropic01_mode check_current_mode(lt_handle_t *h) {

    uint8_t spect_ver[LT_L2_GET_INFO_SPECT_FW_SIZE];

    LT_TEST_ASSERT(LT_OK, lt_get_info_spect_fw_ver(h, spect_ver, LT_L2_GET_INFO_SPECT_FW_SIZE))
    LT_LOG_INFO("Spect ver: %x %x %x %x", spect_ver[0], spect_ver[1], spect_ver[2], spect_ver[3]);
    if (0 == memcmp(spect_ver, "\x00\x00\x00\x80", LT_L2_GET_INFO_SPECT_FW_SIZE)) {
        return LT_BOOTLOADER_MODE;
    } else {
        return LT_NORMAL_MODE;
    }
}

void lt_test_rev_startup_req(void)
{
    LT_LOG_INFO("----------------------------------");
    LT_LOG_INFO("lt_test_rev_startup_req()");
    LT_LOG_INFO("----------------------------------");

    lt_handle_t h;
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(&h));

    // Part 1: Try to reboot from normal to normal.
    LT_LOG_INFO("Checking we are in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode(&h));
    LT_LOG_INFO("Rebooting to the normal mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(&h, LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT));
    LT_LOG_INFO("Checking we are again in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode(&h));

    // Part 2: Try to reboot from normal to bootloader and from bootloader to bootloader.
    for (int i = 0; i < 2; i++) {
        LT_LOG_INFO("Rebooting to the bootloader mode (maintenance reboot)...");
        LT_TEST_ASSERT(LT_OK, lt_reboot(&h, LT_L2_STARTUP_REQ_STARTUP_ID_MAINTENANCE_REBOOT));
        LT_LOG_INFO("Checking we are in the bootloader mode...");
        LT_TEST_ASSERT(LT_BOOTLOADER_MODE, check_current_mode(&h));
        LT_LOG_INFO("Checking that the handshake does not work...");
        LT_TEST_ASSERT(LT_L2_UNKNOWN_REQ, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    }

    // Part 3: Try to reboot from bootloader to normal.
    LT_LOG_INFO("Rebooting to the normal mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(&h, LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT));
    LT_LOG_INFO("Checking we are again in the normal mode...");
    LT_TEST_ASSERT(LT_NORMAL_MODE, check_current_mode(&h));
}