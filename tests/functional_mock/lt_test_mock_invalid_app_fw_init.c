/**
 * @file lt_test_mock_invalid_app_fw_init.c
 * @brief Test for checking that lt_init suceeds if Application FW cannot be booted.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_mock_helpers.h"
#include "lt_test_common.h"

void lt_test_mock_invalid_app_fw_init(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_app_fw_init()");
    LT_LOG_INFO("----------------------------------------------");

    const uint8_t chip_startup_mode = (TR01_L1_CHIP_MODE_READY_bit | TR01_L1_CHIP_MODE_STARTUP_bit);
    const uint8_t lt_get_tr01_mode_mocked_response[] = {chip_startup_mode, TR01_L2_STATUS_NO_RESP};

    lt_mock_hal_reset(&h->l2);

    // 1. Mock lt_init() -> lt_get_tr01_mode() responses (CHIP_STATUS is read twice).
    LT_LOG_INFO("Mocking Get_Response responses...");
    for (int i = 0; i < 2; i++) {
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, lt_get_tr01_mode_mocked_response,
                                                           sizeof(lt_get_tr01_mode_mocked_response)));
    }

    // 2. Mock lt_init() -> lt_reboot() response.
    LT_LOG_INFO("Mocking Startup_Req response...");
    struct lt_l2_startup_rsp_t startup_req_resp = {
        .chip_status = (TR01_L1_CHIP_MODE_READY_bit | TR01_L1_CHIP_MODE_STARTUP_bit),  // Start-up Mode
        .status = TR01_L2_STATUS_REQUEST_OK,
        .rsp_len = TR01_L2_STARTUP_RSP_LEN,
        .crc = {0}  // CRC added below.
    };
    // Add CRC to the Startup_Req response.
    add_resp_crc(&startup_req_resp);

    LT_TEST_ASSERT(
        LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_startup_mode, sizeof(chip_startup_mode)));
    LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&startup_req_resp,
                                                       calc_mocked_resp_len(&startup_req_resp)));

    // 3. Mock lt_init() -> lt_reboot() -> lt_get_tr01_mode() responses (CHIP_STATUS is read twice).
    LT_LOG_INFO("Mocking Get_Response responses...");
    for (int i = 0; i < 2; i++) {
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, lt_get_tr01_mode_mocked_response,
                                                           sizeof(lt_get_tr01_mode_mocked_response)));
    }

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}