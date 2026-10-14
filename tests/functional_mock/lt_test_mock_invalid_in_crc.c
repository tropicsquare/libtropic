/**
 * @file lt_test_mock_invalid_in_crc.c
 * @brief Test for handling invalid CRC in TROPIC01 responses.
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
#include "lt_l3_process.h"
#include "lt_mock_helpers.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"

void lt_test_mock_invalid_in_crc(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_in_crc()");
    LT_LOG_INFO("----------------------------------------------");

    if (LT_CRC_ERR_RETRY_ATTEMPTS < 1) {
        LT_LOG_ERROR("This test requires that at least one retry attempt is configured.");
        LT_TEST_ASSERT(0, 1);  // Forcefully fail the test.
    }

    uint8_t random_byte;

    // 1. Mock init sequence and initialize Libtropic handle.
    // ---------------------------------------------------------------------------------------------

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK, mock_init_communication(
                              h, NULL, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    // 2. L2 path: enqueue Get_Info responses that contain invalid CRCs repeatedly
    //    until retry attempts are exhausted, then verify LT_L2_IN_CRC_ERR is returned.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("2. L2: enqueue Get_Info responses with invalid CRCs for retry tests");

        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        struct lt_l2_get_info_rsp_t get_info_resp_corrupted_crc = {
            .chip_status = TR01_L1_CHIP_MODE_READY_bit,
            .status = TR01_L2_STATUS_REQUEST_OK,
            .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
            .object = {0x00, 0x00, 0x00, 0x02, 0xFF, 0xFF}  // dummy data with invalid CRC appended
        };

        // Enqueue the initial response plus the configured number of retry responses
        // (1 initial + LT_CRC_ERR_RETRY_ATTEMPTS retries).
        for (int i = 0; i < 1 + LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(
                                      &h->l2, (uint8_t *)&get_info_resp_corrupted_crc,
                                      calc_mocked_resp_len(&get_info_resp_corrupted_crc)));
        }

        uint8_t dummy_out[TR01_L2_GET_INFO_RISCV_FW_SIZE];
        LT_TEST_ASSERT(LT_L2_IN_CRC_ERR, lt_get_info_riscv_fw_ver(h, dummy_out));

        // Check IN_CRC error counter and restart it.
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 3. L2 path: send a randomized number of Get_Info frames with invalid CRC,
    //    then a correct frame, and verify the call succeeds (LT_OK).
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("3. L2: enqueue randomized invalid-CRC Get_Info responses, then a valid one");

        // Generate a random count of corrupted frames in range [1, LT_CRC_ERR_RETRY_ATTEMPTS]
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_l2 = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("Sending %d corrupted frames before correct frame", num_corrupted_l2);

        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        struct lt_l2_get_info_rsp_t get_info_resp_corrupted_crc = {
            .chip_status = TR01_L1_CHIP_MODE_READY_bit,
            .status = TR01_L2_STATUS_REQUEST_OK,
            .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
            .object = {0x00, 0x00, 0x00, 0x02, 0xFF, 0xFF}  // dummy data with invalid CRC appended
        };

        // Enqueue random number of corrupted responses
        for (int i = 0; i < num_corrupted_l2; i++) {
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(
                                      &h->l2, (uint8_t *)&get_info_resp_corrupted_crc,
                                      calc_mocked_resp_len(&get_info_resp_corrupted_crc)));
        }

        // Enqueue one correct response
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
        struct lt_l2_get_info_rsp_t get_info_resp = {.chip_status = TR01_L1_CHIP_MODE_READY_bit,
                                                     .status = TR01_L2_STATUS_REQUEST_OK,
                                                     .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
                                                     .object = {0x00, 0x00, 0x00, 0x02}};
        add_resp_crc(&get_info_resp);
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp,
                                                           calc_mocked_resp_len(&get_info_resp)));

        uint8_t dummy_out[TR01_L2_GET_INFO_RISCV_FW_SIZE];
        LT_TEST_ASSERT(LT_OK, lt_get_info_riscv_fw_ver(h, dummy_out));

        // Check IN_CRC error counter and restart it.
        LT_TEST_ASSERT(num_corrupted_l2, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 4. Start mocked Secure Session.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("4. Start mocked Secure Session.");
        uint8_t kcmd[TR01_AES256_KEY_LEN];
        uint8_t kres[TR01_AES256_KEY_LEN];
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, kcmd, sizeof(kcmd)));
        memcpy(kres, kcmd, TR01_AES256_KEY_LEN);
        LT_TEST_ASSERT(LT_OK, mock_session_start(h, kcmd, kres));
    }

    // 5. L3 command path: send command responses with invalid CRCs until retries
    //    are depleted and verify lt_ping returns LT_L2_IN_CRC_ERR.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("5. L3: enqueue command responses containing invalid CRCs for retry test");

        // Enqueue count of responses based on configured resend attempts.
        // 1 + LT_CRC_ERR_RETRY_ATTEMPTS, because first is normal Request, remaining are retries.
        for (int i = 0; i < 1 + LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, true, NULL));
        }

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_L2_IN_CRC_ERR, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Check IN_CRC error counter and restart it.
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 6. L3 command path: send a randomized number of invalid-CRC command responses,
    //    then a correct response and verify lt_ping succeeds (LT_OK).
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("6. L3: enqueue randomized invalid-CRC command responses, then a valid one");

        // Generate random number of corrupted frames in range [1, LT_CRC_ERR_RETRY_ATTEMPTS]
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_l3 = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("Sending %d corrupted frames before correct frame", num_corrupted_l3);

        // Enqueue random number of corrupted responses
        for (int i = 0; i < num_corrupted_l3; i++) {
            LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, true, NULL));
        }

        // Enqueue one correct response
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Mock command result itself.
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, false));

        // Send the command, check the message.
        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_OK, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));
        LT_TEST_ASSERT(0, memcmp(msg_out, msg_in, sizeof(msg_out)));

        // Check IN_CRC error counter and restart it.
        LT_TEST_ASSERT(num_corrupted_l3, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 7. L3 result path: return Results with corrupted CRCs so the receive path
    //    triggers resend handling; deplete retries and expect LT_L2_IN_CRC_ERR.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO("7. L3: sending Results with invalid CRC");

        // Answer with a correct Response frame to the command.
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Return a Result payload that has an invalid CRC.
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, true));

        // Libtropic will try to get correct frame using Resend_Req => enqueue responses to Resend_Req
        // and again a Result with corrupted CRC. Do this multiple times to deplete all attempts.
        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        for (int i = 0; i < LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                                 TR01_L2_STATUS_RESULT_OK, true));
        }

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_L2_IN_CRC_ERR, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Check IN_CRC error counter and restart it.
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 8. L3 result path: send a randomized number of corrupted Results (invalid CRC)
    //    before sending a correct Result and verify lt_ping succeeds (LT_OK).
    // ---------------------------------------------------------------------------------------------
    {
        // Answer with a correct Response frame to the command.
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Generate a random count of corrupted Results in range [1, LT_CRC_ERR_RETRY_ATTEMPTS].
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_l3 = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("8. L3: sending %d corrupted Results before a valid Result", num_corrupted_l3);

        // Send the first corrupted Result.
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, true));

        // Enqueue additional corrupted Results (if any) to simulate resend cycles.
        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        for (int i = 1; i < num_corrupted_l3;
             i++) {  // i = 1 because one corrupted Result already sent
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                                 TR01_L2_STATUS_RESULT_OK, true));
        }

        // Finally, enqueue a valid Result with correct CRC.
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, false));

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_OK, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));
        LT_TEST_ASSERT(0, memcmp(msg_out, msg_in, sizeof(msg_out)));

        // Verify IN_CRC error counter matches number of corrupted Results and reset it.
        LT_TEST_ASSERT(num_corrupted_l3, h->l2.l2_in_crc_error_count);
        h->l2.l2_in_crc_error_count = 0;
    }

    // 9. Terminate the session and deinitialize the Libtropic handle
    // ---------------------------------------------------------------------------------------------
    LT_LOG_INFO("9. Terminate Secure Session and deinitialize the Libtropic handle.");

    LT_LOG_INFO("Terminating the Secure Session...");
    LT_TEST_ASSERT(LT_OK, mock_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}