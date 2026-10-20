/**
 * @file lt_test_mock_invalid_crc.c
 * @brief Test for handling invalid CRC in requests sent to TROPIC01.
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

void lt_test_mock_invalid_crc(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_crc()");
    LT_LOG_INFO("----------------------------------------------");

    if (LT_CRC_ERR_RETRY_ATTEMPTS < 1) {
        LT_LOG_ERROR("This test requires that at least one retry attempt is configured.");
        LT_TEST_ASSERT(0, 1);  // Forcefully fail the test.
    }

    // 1. Mock init sequence and initialize Libtropic handle.
    // ---------------------------------------------------------------------------------------------
    LT_LOG_INFO("1. Mock init sequence and initialize Libtropic handle.");

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK,
                   mock_init_communication(h, &mock_chip_id_acab,
                                           (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    // 2. L2 path: return STATUS=CRC_ERR until retries are depleted and verify LT_L2_CRC_ERR.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "2. L2 path: return STATUS=CRC_ERR until retries are depleted and verify LT_L2_CRC_ERR.");

        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        struct lt_l2_get_info_rsp_t get_info_resp_crc_error_status = {
            .chip_status = TR01_L1_CHIP_MODE_READY_bit,
            .status = TR01_L2_STATUS_CRC_ERR,
            .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
            .object = {0x00, 0x00, 0x00, 0x02}  // dummy data
        };
        add_resp_crc(&get_info_resp_crc_error_status);

        // Enqueue one initial response plus all retry responses.
        // Total count is 1 + LT_CRC_ERR_RETRY_ATTEMPTS.
        for (int i = 0; i < 1 + LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(
                                      &h->l2, (uint8_t *)&get_info_resp_crc_error_status,
                                      calc_mocked_resp_len(&get_info_resp_crc_error_status)));
        }

        uint8_t dummy_out[TR01_L2_GET_INFO_RISCV_FW_SIZE];
        LT_TEST_ASSERT(LT_L2_CRC_ERR, lt_get_info_riscv_fw_ver(h, dummy_out));

        // Verify error counters and reset CRC counter for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
    }
    // 3. L2 path: return random number of STATUS=CRC_ERR frames, then STATUS=OK, and verify LT_OK.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "3. L2 path: return random number of STATUS=CRC_ERR frames, then STATUS=OK, and verify "
            "LT_OK.");

        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        struct lt_l2_get_info_rsp_t get_info_resp_crc_error_status = {
            .chip_status = TR01_L1_CHIP_MODE_READY_bit,
            .status = TR01_L2_STATUS_CRC_ERR,
            .rsp_len = TR01_L2_GET_INFO_RISCV_FW_SIZE,
            .object = {0x00, 0x00, 0x00, 0x02}  // dummy data
        };
        add_resp_crc(&get_info_resp_crc_error_status);

        uint8_t random_byte;
        // Generate number of CRC_ERR responses in range [1, LT_CRC_ERR_RETRY_ATTEMPTS].
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_l2 = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("Sending %d corrupted frames before correct frame", num_corrupted_l2);

        // Enqueue randomized number of CRC_ERR responses.
        for (int i = 0; i < num_corrupted_l2; i++) {
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(
                                      &h->l2, (uint8_t *)&get_info_resp_crc_error_status,
                                      calc_mocked_resp_len(&get_info_resp_crc_error_status)));
        }

        // Enqueue one STATUS=OK response.
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

        // Verify error counters and reset CRC counter for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(num_corrupted_l2, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
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

    // 5. L3 command path (lt_l2_send_encrypted_cmd): return STATUS=CRC_ERR until retries are depleted
    // and verify LT_L2_CRC_ERR from lt_ping.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "5. L3 command path (lt_l2_send_encrypted_cmd): return STATUS=CRC_ERR until retries are "
            "depleted and verify LT_L2_CRC_ERR from lt_ping.");

        // Enqueue one initial response plus all retry responses.
        // Total count is 1 + LT_CRC_ERR_RETRY_ATTEMPTS.
        const uint8_t resp_with_crc_error_status[5] = {TR01_L1_CHIP_MODE_READY_bit,
                                                       TR01_L2_STATUS_CRC_ERR, 0x00};

        for (int i = 0; i < 1 + LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, resp_with_crc_error_status));
        }

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_L2_CRC_ERR, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Verify error counters and reset CRC counter for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
    }
    // 6. L3 command path (lt_l2_send_encrypted_cmd): return random number of STATUS=CRC_ERR frames,
    // then STATUS=OK, and verify LT_OK from lt_ping.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "6. L3 command path (lt_l2_send_encrypted_cmd): return random number of STATUS=CRC_ERR "
            "frames, then STATUS=OK, and verify LT_OK from lt_ping.");

        // Generate number of CRC_ERR responses in range [1, LT_CRC_ERR_RETRY_ATTEMPTS].
        uint8_t random_byte;
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_l2 = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("Sending %d corrupted frames before correct frame", num_corrupted_l2);

        // Enqueue randomized number of CRC_ERR responses.
        const uint8_t resp_with_crc_error_status[5] = {TR01_L1_CHIP_MODE_READY_bit,
                                                       TR01_L2_STATUS_CRC_ERR, 0x00};

        for (int i = 0; i < num_corrupted_l2; i++) {
            LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, resp_with_crc_error_status));
        }
        // Enqueue frame with STATUS=OK.
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Enqueue successful L3 result payload returned by lt_ping.
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, false));

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_OK, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Verify error counters and reset CRC counter for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(num_corrupted_l2, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
    }

    // 7. L3 result path (lt_l2_recv_encrypted_res): return frames with STATUS=CRC_ERR until
    // retries are depleted and verify LT_L2_CRC_ERR from lt_ping.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "7. L3 result path (lt_l2_recv_encrypted_res): return frames with STATUS=CRC_ERR until "
            "retries are depleted and verify LT_L2_CRC_ERR from lt_ping.");

        // Mock a command response (STATUS=OK)
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Enqueue L3 result with STATUS=CRC_ERR.
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_CRC_ERR, false));

        // Enqueue responses to Resend_Req and L3 results with STATUS=CRC_ERR.
        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        for (int i = 0; i < LT_CRC_ERR_RETRY_ATTEMPTS; i++) {
            // Response to Resend_Req.
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
            // Resent frame, again with STATUS=CRC_ERR.
            LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                                 TR01_L2_STATUS_CRC_ERR, false));
        }

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_L2_CRC_ERR, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Verify error counter and reset for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(1 + LT_CRC_ERR_RETRY_ATTEMPTS, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
    }

    // 8. L3 result path (lt_l2_recv_encrypted_res): return random number of frames with
    // STATUS=CRC_ERR, then a correct frame, and verify LT_OK from lt_ping.
    // ---------------------------------------------------------------------------------------------
    {
        LT_LOG_INFO(
            "8. L3 result path (lt_l2_recv_encrypted_res): return random number of frames with "
            "STATUS=CRC_ERR, then a correct frame, and verify LT_OK from lt_ping.");

        // Generate number of CRC_ERR responses in range [1, LT_CRC_ERR_RETRY_ATTEMPTS].
        uint8_t random_byte;
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_byte, sizeof(random_byte)));
        int num_corrupted_results = (random_byte % LT_CRC_ERR_RETRY_ATTEMPTS) + 1;
        LT_LOG_INFO("Sending %d corrupted result frames before correct frame", num_corrupted_results);

        // Mock a command response (STATUS=OK)
        LT_TEST_ASSERT(LT_OK, mock_l3_command_responses(h, 1, false, NULL));

        // Enqueue responses to Resend_Req and L3 results with STATUS=CRC_ERR.
        const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
        uint8_t lt_ping_plaintext[] = {TR01_L3_RESULT_OK, 'H', 'E', 'L', 'L', 'O'};
        for (int i = 0; i < num_corrupted_results; i++) {
            // Enqueue L3 result with STATUS=CRC_ERR.
            LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                                 TR01_L2_STATUS_CRC_ERR, false));

            // Response to Resend_Req.
            LT_TEST_ASSERT(LT_OK,
                           lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
        }

        // Enqueue successful L3 result payload returned by lt_ping.
        LT_TEST_ASSERT(LT_OK, mock_l3_result(h, lt_ping_plaintext, sizeof(lt_ping_plaintext),
                                             TR01_L2_STATUS_RESULT_OK, false));

        const uint8_t msg_out[] = {'H', 'E', 'L', 'L', 'O'};
        uint8_t msg_in[sizeof(msg_out)];
        LT_TEST_ASSERT(LT_OK, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

        // Verify error counter and reset for next test step.
        LT_TEST_ASSERT(0, h->l2.l2_in_crc_error_count);
        LT_TEST_ASSERT(num_corrupted_results, h->l2.l2_crc_error_count);
        h->l2.l2_crc_error_count = 0;
    }

    // 9. Terminate Secure Session and deinitialize the Libtropic handle.
    // ---------------------------------------------------------------------------------------------
    LT_LOG_INFO("9. Terminate Secure Session and deinitialize the Libtropic handle.");

    LT_LOG_INFO("Terminating the Secure Session...");
    LT_TEST_ASSERT(LT_OK, mock_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}