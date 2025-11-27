/**
 * @file lt_test_mock_attrs.c
 * @brief Example functional test using `hal/mock` to test attribute structure handling.
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_functional_tests.h"
#include "libtropic_l2.h"
#include "lt_crc16.h"
#include "lt_l1.h"
#include "lt_l2_frame_check.h"
#include "lt_l2_api_structs.h"

/* Mock control API */
#include "libtropic_port_mock.h"
#include "lt_mock_helpers.h"

int lt_test_mock_attrs(lt_handle_t *h)
{
    LT_LOG_INFO("Starting lt_test_mock_attrs...");

    uint8_t riscv_fw_ver_resp[][TR01_L2_GET_INFO_RISCV_FW_SIZE] = {
        {0x00, 0x00, 0x00, 0x02}, // Version 2.0.0
        {0x00, 0x01, 0x00, 0x01}, // Version 1.0.1
        {0x00, 0x00, 0x00, 0x01}, // Version 1.0.0
        {0x00, 0x00, 0x05, 0x00}  // Version 0.5.0
    };

    for (size_t i = 0; i < sizeof(riscv_fw_ver_resp)/sizeof(riscv_fw_ver_resp[0]); i++) {
        LT_LOG_INFO("Testing with mocked RISC-V FW version: %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "",
                    riscv_fw_ver_resp[i][3],
                    riscv_fw_ver_resp[i][2],
                    riscv_fw_ver_resp[i][1],
                    riscv_fw_ver_resp[i][0]);

        lt_mock_hal_reset(&h->l2);

        // Mock response data for chip mode check.
        uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready));

        // Mock response data for Get_Info, for both L2 Request.
        lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready));

        struct lt_l2_get_info_rsp_t get_info_resp = {
            .chip_status = TR01_L1_CHIP_MODE_READY_bit,
            .status = TR01_L2_STATUS_REQUEST_OK,
            .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
            .object = {0}
        };
        memcpy(get_info_resp.object, riscv_fw_ver_resp[i], TR01_L2_GET_INFO_RISCV_FW_SIZE);
        add_resp_crc(&get_info_resp);
        // Mock response data for Get_Info, for both L2 Response.
        lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp, calc_mocked_resp_len(&get_info_resp));

        LT_LOG_INFO("Initializing handle");
        LT_ASSERT(LT_OK, lt_init(h));

        LT_LOG_INFO("Checking if attributes were set correctly");
        if (riscv_fw_ver_resp[i][3] < 2) {
            LT_ASSERT(h->tr01_attrs.r_mem_udata_slot_size_max, 444);
        } else {
            LT_ASSERT(h->tr01_attrs.r_mem_udata_slot_size_max, 475);
        }

        LT_LOG_INFO("Deinitializing handle");
        LT_ASSERT(LT_OK, lt_deinit((lt_handle_t *)h));
    }

    return 0;
}
