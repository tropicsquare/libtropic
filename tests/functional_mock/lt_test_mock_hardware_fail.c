/**
 * @file lt_test_mock_hardware_fail.c
 * @brief Test HARDWARE_FAIL L3 Result handling.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_l2.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port_mock.h"
#include "lt_crc16.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_l3_process.h"
#include "lt_mock_helpers.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"

void lt_test_mock_hardware_fail(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_hardware_fail()");
    LT_LOG_INFO("----------------------------------------------");

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK,
                   mock_init_communication(h, &mock_chip_id_acab,
                                           (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Setting up session...");
    uint8_t kcmd[TR01_AES256_KEY_LEN];
    uint8_t kres[TR01_AES256_KEY_LEN];
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, kcmd, sizeof(kcmd)));
    memcpy(kres, kcmd, TR01_AES256_KEY_LEN);
    LT_TEST_ASSERT(LT_OK, mock_session_start(h, kcmd, kres));

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Mocking HARDWARE_FAIL in Pairing_Key_Write reply...");
    uint8_t dummy_key[TR01_SHIPUB_LEN];
    for (int slot = TR01_PAIRING_KEY_SLOT_INDEX_0; slot <= TR01_PAIRING_KEY_SLOT_INDEX_3; slot++) {
        LT_LOG_INFO("Mocking for slot %d...", slot);
        // Mock replies to the command.
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Mock command result itself.
        uint8_t pairing_key_write_plaintext[] = {
            TR01_L3_RESULT_HARDWARE_FAIL,
        };
        LT_TEST_ASSERT(
            LT_OK, mock_l3_result(h, pairing_key_write_plaintext, sizeof(pairing_key_write_plaintext),
                                  TR01_L2_STATUS_RESULT_OK, false));

        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, dummy_key, sizeof(dummy_key)));
        LT_TEST_ASSERT(LT_L3_HARDWARE_FAIL, lt_pairing_key_write(h, dummy_key, slot));
    }

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Mocking HARDWARE_FAIL in Pairing_Key_Invalidate reply...");
    for (int slot = TR01_PAIRING_KEY_SLOT_INDEX_0; slot <= TR01_PAIRING_KEY_SLOT_INDEX_3; slot++) {
        LT_LOG_INFO("Mocking for slot %d...", slot);
        // Mock replies to the command.
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Mock command result itself.
        uint8_t pairing_key_invalidate_plaintext[] = {
            TR01_L3_RESULT_HARDWARE_FAIL,
        };
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, pairing_key_invalidate_plaintext,
                                             sizeof(pairing_key_invalidate_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, false));
        LT_TEST_ASSERT(LT_L3_HARDWARE_FAIL, lt_pairing_key_invalidate(h, slot));
    }

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Mocking HARDWARE_FAIL in R_Config_Write reply...");
    // Mock replies to the command.
    LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

    // Mock command result itself.
    uint8_t r_config_write_plaintext[] = {
        TR01_L3_RESULT_HARDWARE_FAIL,
    };
    LT_TEST_ASSERT(LT_OK, mock_l3_result(h, r_config_write_plaintext, sizeof(r_config_write_plaintext),
                                         TR01_L2_STATUS_RESULT_OK, false));
    LT_TEST_ASSERT(LT_L3_HARDWARE_FAIL,
                   lt_r_config_write(h, TR01_CFG_START_UP_ADDR, 0x00));  // Dummy object

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Mocking HARDWARE_FAIL in I_Config_Write reply...");
    // Mock replies to the command.
    LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

    // Mock command result itself.
    uint8_t i_config_write_plaintext[] = {
        TR01_L3_RESULT_HARDWARE_FAIL,
    };
    LT_TEST_ASSERT(LT_OK, mock_l3_result(h, i_config_write_plaintext, sizeof(i_config_write_plaintext),
                                         TR01_L2_STATUS_RESULT_OK, false));
    LT_TEST_ASSERT(LT_L3_HARDWARE_FAIL,
                   lt_i_config_write(h, TR01_CFG_START_UP_ADDR, 0x00));  // Dummy object

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Mocking HARDWARE_FAIL in R_Mem_Data_Write reply...");
    // Mock replies to the command.
    LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

    // Mock command result itself.
    uint8_t r_mem_data_write_plaintext[] = {
        TR01_L3_RESULT_HARDWARE_FAIL,
    };
    LT_TEST_ASSERT(LT_OK,
                   mock_l3_result(h, r_mem_data_write_plaintext, sizeof(r_mem_data_write_plaintext),
                                  TR01_L2_STATUS_RESULT_OK, false));

    uint16_t random_r_mem_slot;
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_r_mem_slot, sizeof(random_r_mem_slot)));
    random_r_mem_slot %= TR01_R_MEM_DATA_SLOT_MAX + 1;
    uint8_t random_r_mem_data[16];  // Arbitrary size. Exact number not important, we just need some
                                    // dummy data.
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, random_r_mem_data, sizeof(random_r_mem_data)));
    LT_TEST_ASSERT(LT_L3_HARDWARE_FAIL, lt_r_mem_data_write(h, random_r_mem_slot, random_r_mem_data,
                                                            sizeof(random_r_mem_data)));

    // ----------------------------------------------------------------------------------------------------------

    LT_LOG_INFO("Terminating the Secure Session...");
    LT_TEST_ASSERT(LT_OK, mock_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
