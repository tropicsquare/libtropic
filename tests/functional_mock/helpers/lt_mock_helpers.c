/**
 * @file lt_mock_helpers.c
 * @brief Helper functions for functional mock tests.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "lt_mock_helpers.h"

#include <memory.h>
#include <stdlib.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_aesgcm.h"
#include "lt_crc16.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_l3_process.h"

void add_resp_crc(void *resp_buf)
{
    uint8_t *resp_buf_bytes = (uint8_t *)resp_buf;

    // CRC is calculated from STATUS, RSP_LEN and RSP_DATA, skipping CHIP_STATUS.
    uint16_t resp_len = TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE +
                        resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET];
    uint16_t crc = crc16(resp_buf_bytes + TR01_L1_CHIP_STATUS_SIZE, resp_len);

    // Append CRC at the end of the response buffer.
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len] = crc >> 8;
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len + 1] = crc & 0x00FF;
}

size_t calc_mocked_resp_len(const void *resp_buf)
{
    const uint8_t *resp_buf_bytes = (const uint8_t *)resp_buf;

    // Total length is CHIP_STATUS + STATUS + RSP_LEN + RSP_DATA + CRC
    return TR01_L1_CHIP_STATUS_SIZE + TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE +
           resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET] + TR01_L2_REQ_RSP_CRC_SIZE;
}

/**
 * @brief Mocks both L2 Request and L2 Response of a single Get_Info Request.
 *
 * @param h Pointer to the lt_handle_t structure.
 * @param object Data of the requested object block to return.
 * @param object_size Size of the object (at most 128 bytes).
 *
 * @return lt_ret_t LT_OK on success, error code otherwise.
 */
static lt_ret_t mock_get_info(lt_handle_t *h, const void *object, const uint8_t object_size)
{
    uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;

    // Mock response data for Get_Info, for the L2 Request.
    if (LT_OK != lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready))) {
        return LT_FAIL;
    }

    struct lt_l2_get_info_rsp_t get_info_resp = {.chip_status = TR01_L1_CHIP_MODE_READY_bit,
                                                 .status = TR01_L2_STATUS_REQUEST_OK,
                                                 .rsp_len = object_size,
                                                 .object = {0}};
    memcpy(get_info_resp.object, object, object_size);
    add_resp_crc(&get_info_resp);

    // Mock response data for Get_Info, for the L2 Response.
    if (LT_OK != lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp,
                                              calc_mocked_resp_len(&get_info_resp))) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t mock_init_communication_chip_id(lt_handle_t *h, const struct lt_chip_id_t *chip_id,
                                         const uint8_t riscv_fw_ver[4])
{
    // Mock response data for chip mode check.
    uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;

    if (LT_OK != lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready))) {
        return LT_FAIL;
    }

    // Mock Get_Info for CHIP_ID (used to determine the silicon revision).
    if (LT_OK != mock_get_info(h, chip_id, TR01_L2_GET_INFO_CHIP_ID_SIZE)) {
        return LT_FAIL;
    }

    // Mock Get_Info for the RISC-V FW version.
    if (LT_OK != mock_get_info(h, riscv_fw_ver, TR01_L2_GET_INFO_RISCV_FW_SIZE)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t mock_init_communication(lt_handle_t *h, const uint8_t riscv_fw_ver[4])
{
    const struct lt_chip_id_t chip_id = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                                         .silicon_rev = {'A', 'C', 'A', 'B'}};

    return mock_init_communication_chip_id(h, &chip_id, riscv_fw_ver);
}

lt_ret_t mock_session_start(lt_handle_t *h, const uint8_t kcmd[TR01_AES256_KEY_LEN],
                            const uint8_t kres[TR01_AES256_KEY_LEN])
{
    // Check if kcmd and kres are equal.
    // This is needed so we can reuse AES-GCM functions currently provided by CAL and not having to
    // reinitialize AES with swapped encryption and decryption keys every time.
    if (memcmp(kcmd, kres, TR01_AES256_KEY_LEN) != 0) {
        LT_LOG_ERROR("kcmd and kres have to match for L3 mocking to work (simplification).");
        return LT_PARAM_ERR;
    }

    // Initialize IVs.
    lt_l3_invalidate_host_session_data(&h->l3);

    // Initialize AES-GCM with provided keys.
    lt_ret_t ret = lt_aesgcm_encrypt_init(h->l3.crypto_ctx, kcmd, TR01_AES256_KEY_LEN);
    if (ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }
    ret = lt_aesgcm_decrypt_init(h->l3.crypto_ctx, kres, TR01_AES256_KEY_LEN);
    if (ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    // Mark session status as started.
    h->l3.session_status = LT_SECURE_SESSION_ON;

    return LT_OK;
}

lt_ret_t mock_session_abort(lt_handle_t *h)
{
    lt_l3_invalidate_host_session_data(&h->l3);

    return LT_OK;
}

lt_ret_t mock_l3_result(lt_handle_t *h, const uint8_t *result_plaintext,
                        const size_t result_plaintext_size, const uint8_t status,
                        const bool corrupt_crc)
{
    uint8_t l2_frame[TR01_L2_MAX_FRAME_SIZE];

    size_t packet_size = TR01_L3_SIZE_SIZE + result_plaintext_size + TR01_L3_TAG_SIZE;
    size_t frame_size = TR01_L1_CHIP_STATUS_SIZE + TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE +
                        packet_size + TR01_L2_REQ_RSP_CRC_SIZE;

    if (packet_size > TR01_L2_CHUNK_MAX_DATA_SIZE) {
        LT_LOG_ERROR("Payloads >%u B not supported due to chunking not implemented.",
                     TR01_L2_CHUNK_MAX_DATA_SIZE);
        return LT_PARAM_ERR;
    }

    // This will happen only if the internal macros are implemented incorrectly.
    if (frame_size > TR01_L2_MAX_FRAME_SIZE) {
        LT_LOG_ERROR(
            "Implementation error! Total frame size won't fit to the buffer. Need at least: %zu",
            frame_size);
        return LT_FAIL;
    }

    l2_frame[TR01_L2_CHIP_STATUS_OFFSET] = TR01_L1_CHIP_MODE_READY_bit;
    l2_frame[TR01_L2_STATUS_OFFSET] = status;
    l2_frame[TR01_L2_RSP_LEN_OFFSET] = (uint8_t)packet_size;

    l2_frame[TR01_L2_RSP_DATA_RSP_CRC_OFFSET] = result_plaintext_size;
    l2_frame[TR01_L2_RSP_DATA_RSP_CRC_OFFSET + 1] = 0x00;

    lt_ret_t ret = lt_aesgcm_encrypt(h->l3.crypto_ctx, h->l3.decryption_IV, TR01_L3_IV_SIZE, NULL, 0,
                                     result_plaintext, result_plaintext_size,
                                     &l2_frame[TR01_L2_RSP_DATA_RSP_CRC_OFFSET + TR01_L3_SIZE_SIZE],
                                     result_plaintext_size + TR01_L3_TAG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Encryption failed! ret=%d", ret);
        return ret;
    }
    // As the mock helpers share CAL interface with Libtropic (simplification), IV is handled in the
    // Libtropic itself -> no need to increment here.

    uint16_t crc = crc16(&l2_frame[TR01_L2_STATUS_OFFSET],
                         TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE + packet_size);
    if (corrupt_crc) {
        crc = ~crc;
    }
    size_t crc_offset = TR01_L2_RSP_DATA_RSP_CRC_OFFSET + packet_size;
    l2_frame[crc_offset] = crc >> 8;
    l2_frame[crc_offset + 1] = crc & 0x00FF;

    ret = lt_mock_hal_enqueue_response(&h->l2, l2_frame, frame_size);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to enqueue response with L3 Result!");
        return ret;
    }

    return LT_OK;
}

lt_ret_t mock_l3_command_responses(lt_handle_t *h, const size_t chunk_count, const bool corrupt_crc,
                                   const uint8_t custom_resp_frame[5])
{
    if (chunk_count > 1) {
        LT_LOG_ERROR("Only single chunk supported now!");
        return LT_PARAM_ERR;
    }

    uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;
    lt_ret_t ret = lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready));
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to enqueue L3 Command response 1/2 (CHIP_READY).");
        return ret;
    }

    uint8_t req_ok_frame[5] = {0};
    if (custom_resp_frame == NULL) {
        req_ok_frame[TR01_L2_CHIP_STATUS_OFFSET] = TR01_L1_CHIP_MODE_READY_bit;
        req_ok_frame[TR01_L2_STATUS_OFFSET] = TR01_L2_STATUS_REQUEST_OK;
        req_ok_frame[TR01_L2_RSP_LEN_OFFSET] = 0x00;
    }
    else {
        memcpy(req_ok_frame, custom_resp_frame, sizeof(req_ok_frame));
    }

    uint16_t crc = crc16(req_ok_frame + 1, 2);

    if (corrupt_crc) {
        crc = ~crc;
    }

    req_ok_frame[TR01_L2_RSP_DATA_RSP_CRC_OFFSET] = crc >> 8;
    req_ok_frame[TR01_L2_RSP_DATA_RSP_CRC_OFFSET + 1] = crc & 0x00FF;

    ret = lt_mock_hal_enqueue_response(&h->l2, req_ok_frame, sizeof(req_ok_frame));
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to enqueue L3 Command response 2/2 (L2 Response)");
        return ret;
    }

    return LT_OK;
}
