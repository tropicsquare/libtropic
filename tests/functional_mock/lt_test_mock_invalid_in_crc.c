/**
 * @file lt_test_mock_invalid_in_crc.c
 * @brief Test for handling invalid CRC in TROPIC01 responses.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_l1.h"
#include "lt_l2_frame_check.h"
#include "lt_l2_api_structs.h"
#include "libtropic_port_mock.h"
#include "libtropic_logging.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "lt_mock_helpers.h"

#include "lt_functional_mock_tests.h"

int lt_test_mock_invalid_in_crc(lt_handle_t *h) {

    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_in_crc()");
    LT_LOG_INFO("----------------------------------------------");

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_ASSERT(LT_OK, mock_init_communication(h, (uint8_t[]){0x00, 0x00, 0x00, 0x02})); // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(h));

    uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
    LT_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
    struct lt_l2_get_info_rsp_t get_info_resp = {
        .chip_status = TR01_L1_CHIP_MODE_READY_bit,
        .status = TR01_L2_STATUS_REQUEST_OK,
        .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
        .object = {0x00, 0x00, 0x00, 0x02, 0xFF, 0xFF} // dummy data with invalid CRC appended
    };
    LT_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp, calc_mocked_resp_len(&get_info_resp)));

    LT_LOG_INFO("Sending Get_Info request with invalid CRC in response...");
    uint8_t dummy_out[TR01_L2_GET_INFO_RISCV_FW_SIZE];
    LT_ASSERT(LT_L2_IN_CRC_ERR, lt_get_info_riscv_fw_ver(h, dummy_out));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(h));

    return 0;
}