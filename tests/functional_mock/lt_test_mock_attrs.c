/**
 * @file lt_test_mock_attrs.c
 * @brief Test for checking if TROPIC01 attributes are set correctly based on RISC-V FW version.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_l2.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port_mock.h"
#include "lt_crc16.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_mock_helpers.h"

int lt_test_mock_attrs(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_attrs()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t riscv_fw_ver_resp[][TR01_L2_GET_INFO_RISCV_FW_SIZE] = {
        {0x00, 0x00, 0x00, 0x02},  // Version 2.0.0
        {0x00, 0x01, 0x00, 0x01},  // Version 1.0.1
        {0x00, 0x00, 0x00, 0x01},  // Version 1.0.0
        {0x00, 0x00, 0x05, 0x00}   // Version 0.5.0
    };

    for (size_t i = 0; i < sizeof(riscv_fw_ver_resp) / sizeof(riscv_fw_ver_resp[0]); i++) {
        LT_LOG_INFO("Testing with mocked RISC-V FW version: %" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "",
                    riscv_fw_ver_resp[i][3], riscv_fw_ver_resp[i][2], riscv_fw_ver_resp[i][1], riscv_fw_ver_resp[i][0]);

        lt_mock_hal_reset(&h->l2);
        LT_LOG_INFO("Mocking initialization...");
        LT_ASSERT(LT_OK, mock_init_communication(h, riscv_fw_ver_resp[i]));

        LT_LOG_INFO("Initializing handle");
        LT_ASSERT(LT_OK, lt_init(h));

        LT_LOG_INFO("Checking if attributes were set correctly");
        if (riscv_fw_ver_resp[i][3] < 2) {
            LT_ASSERT(h->tr01_attrs.r_mem_udata_slot_size_max, 444);
        }
        else {
            LT_ASSERT(h->tr01_attrs.r_mem_udata_slot_size_max, 475);
        }

        LT_LOG_INFO("Deinitializing handle");
        LT_ASSERT(LT_OK, lt_deinit((lt_handle_t *)h));
    }

    return 0;
}
