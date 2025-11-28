/**
 * @file lt_mock_helpers.c
 * @brief Helper functions for functional mock tests.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_mock_helpers.h"

#include <memory.h>
#include <stdlib.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_crc16.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"

void add_resp_crc(void *resp_buf)
{
    uint8_t *resp_buf_bytes = (uint8_t *)resp_buf;

    // CRC is calculated from STATUS, RSP_LEN and RSP_DATA, skipping CHIP_STATUS.
    uint16_t resp_len = TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE + resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET];
    uint16_t crc = crc16(resp_buf_bytes + TR01_L1_CHIP_STATUS_SIZE, resp_len);

    // Append CRC at the end of the response buffer.
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len] = crc >> 8;
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len + 1] = crc & 0x00FF;
}

size_t calc_mocked_resp_len(const void *resp_buf)
{
    const uint8_t *resp_buf_bytes = (const uint8_t *)resp_buf;

    // Total length is CHIP_STATUS + STATUS + RSP_LEN + RSP_DATA + CRC
    return TR01_L1_CHIP_STATUS_SIZE + TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE
           + resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET] + TR01_L2_REQ_RSP_CRC_SIZE;
}

lt_ret_t mock_init_communication(lt_handle_t *h, const uint8_t riscv_fw_ver[4])
{
    // Mock response data for chip mode check.
    uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;

    if (LT_OK != lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready))) {
        return LT_FAIL;
    }

    // Mock response data for Get_Info, for both L2 Request.
    if (LT_OK != lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready))) {
        return LT_FAIL;
    }

    struct lt_l2_get_info_rsp_t get_info_resp = {.chip_status = TR01_L1_CHIP_MODE_READY_bit,
                                                 .status = TR01_L2_STATUS_REQUEST_OK,
                                                 .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
                                                 .object = {0}};
    memcpy(get_info_resp.object, riscv_fw_ver, TR01_L2_GET_INFO_RISCV_FW_SIZE);
    add_resp_crc(&get_info_resp);

    // Mock response data for Get_Info, for both L2 Response.
    if (LT_OK
        != lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp, calc_mocked_resp_len(&get_info_resp))) {
        return LT_FAIL;
    }
}
