/**
 * @file TODO: FILL ME
 * @brief TODO: FILL ME
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <string.h>

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

void lt_test_mock_invalid_pn(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_pn()");
    LT_LOG_INFO("----------------------------------------------");

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK,
                   mock_init_communication(h, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    const lt_chip_id_t chip_id_invalid_pn = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                                             .silicon_rev = {'A', 'C', 'A', 'B'},
                                             .part_num_data = {0xff}};

    struct lt_l2_get_info_rsp_t get_info_resp_invalid_pn = {.chip_status = TR01_L1_CHIP_MODE_READY_bit,
                                                            .status = TR01_L2_STATUS_REQUEST_OK,
                                                            .rsp_len = TR01_L2_GET_INFO_CHIP_ID_SIZE,
                                                            .crc = {0}};
    memcpy(get_info_resp_invalid_pn.object, &chip_id_invalid_pn, sizeof(chip_id_invalid_pn));
    // Add CRC to the Startup_Req response.
    add_resp_crc(&get_info_resp_invalid_pn);

    LT_TEST_ASSERT(LT_FAIL,
                   lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp_invalid_pn,
                                                calc_mocked_resp_len(&get_info_resp_invalid_pn)));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}