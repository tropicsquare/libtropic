#ifndef LT_MOCK_HELPERS_H
#define LT_MOCK_HELPERS_H

/**
 * @file lt_mock_helpers.h
 * @brief Helper functions for functional mock tests.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds CRC16 to the end of the mocked response buffer.
 *
 * @param resp_buf Pointer to the response buffer where CRC will be appended. The buffer must have
 * enough space for the CRC (2 bytes).
 */
void add_resp_crc(void *resp_buf);

/**
 * @brief Calculates the total length of a mocked response buffer, including CHIP_STATUS, STATUS,
 * RSP_LEN, RSP_DATA, and CRC.
 *
 * @param resp_buf Pointer to the response buffer.
 * @return size_t Total length of the mocked response buffer.
 */
size_t calc_mocked_resp_len(const void *resp_buf);

/**
 * @brief Mock all data required to initialize Libtropic with lt_init().
 *
 * @details Currently, lt_init() initializes attributes based on the Application FW version running on
 * TROPIC01. The version is obtained from the chip. This function mocks the necessary responses to
 * simulate a specific FW version. As this operation needs to be done in each test that does any
 * communication, this helper function is provided to simplify the process.
 *
 * @note CHIP_ID with silicon revision ACAB is mocked. Use mock_init_communication_chip_id() to mock
 * a custom CHIP_ID.
 *
 * @param h Pointer to the lt_handle_t structure.
 * @param riscv_fw_ver Array representing the desired RISC-V FW version to mock.
 *
 * @return lt_ret_t LT_OK on success, error code otherwise.
 */
lt_ret_t mock_init_communication(lt_handle_t *h, const uint8_t riscv_fw_ver[4]);

/**
 * @brief Mock all data required to initialize Libtropic with lt_init(), using a custom CHIP_ID.
 *
 * @details Same as mock_init_communication(), but allows to specify the CHIP_ID returned by
 * TROPIC01 (e.g. to simulate different silicon revisions).
 *
 * @param h Pointer to the lt_handle_t structure.
 * @param chip_id CHIP_ID to mock.
 * @param riscv_fw_ver Array representing the desired RISC-V FW version to mock.
 *
 * @return lt_ret_t LT_OK on success, error code otherwise.
 */
lt_ret_t mock_init_communication_chip_id(lt_handle_t *h, const struct lt_chip_id_t *chip_id,
                                         const uint8_t riscv_fw_ver[4]);

/**
 * @brief Initialize and start a mocked Secure Session for functional mock tests.
 *
 * Prepares the provided handle for use in mocked TR01 operations, using the
 * supplied AES-256 keys for L3 Result encryption. This does not simulate
 * communication of Secure Session handshake, just prepares the handle.
 *
 * On success, *h is initialized as if the Secure Session handshake occurred.
 *
 * @param h Pointer to an lt_handle_t that will be initialized. Must be non-NULL.
 * @param kcmd Buffer of TR01_AES256_KEY_LEN bytes containing the AES-256 key used for
 *     command encryption. `kcmd` and `kres` must be equal.
 * @param kres Buffer of TR01_AES256_KEY_LEN bytes containing the AES-256 key used for
 *     response encryption. `kcmd` and `kres` must be equal.
 *
 * @return LT_OK on success, or an appropriate lt_ret_t error code on failure.
 */
lt_ret_t mock_session_start(lt_handle_t *h, const uint8_t kcmd[TR01_AES256_KEY_LEN],
                            const uint8_t kres[TR01_AES256_KEY_LEN]);

/**
 * @brief Abort mocked Secure Session.
 *
 * Removes all data relevant to the Secure Session and modifies the handle
 * as if the lt_session_abort was called, but does not actually simulate any communication.
 *
 * @param h Pointer to an lt_handle_t to be modified.
 *
 * @return LT_OK on success, or an appropriate lt_ret_t error code on failure.
 */
lt_ret_t mock_session_abort(lt_handle_t *h);

/**
 * @brief Mock whole L2 Frame containing the L3 Result as a reply to any Libtropic's L3 Command.
 *
 * You only need to provide plaintext part (RESULT field + any data if applicable). It will
 * be encrypted, tag will be added and inserted to an appropriate L2 Response frame.
 *
 * @warning Currently only Results that fit to a single chunk are supported, as chunking
 * is not implemented yet.
 *
 * @param h Pointer to an lt_handle_t to use (for encryption and enqueuing).
 * @param result_plaintext Plaintext of the L3 Result data to use.
 * @param result_plaintext_size Size of the result_plaintext.
 * @param status Value of the STATUS field of the frame.
 * @param corrupt_crc If true, CRC will be intentionally corrupted (e.g., to test CRC handling).
 *
 * @return LT_OK on success, or an appropriate lt_ret_t error code on failure.
 */
lt_ret_t mock_l3_result(lt_handle_t *h, const uint8_t *result_plaintext,
                        const size_t result_plaintext_size, const uint8_t status,
                        const bool corrupt_crc);

/**
 * @brief Mock replies to a L3 Command.
 *
 * There are two types of replies from TROPIC01 to each L3 Command chunk:
 *   1. reply to writing the chunk (using Encrypted_Cmd_Req L2 Request) -> only CHIP_READY (as to other
 * L2 Requests)
 *   2. reply to Get_Response -> L2 Response with status REQ_OK (last chunk) or REQ_CONT (not the last
 * chunk)
 *
 * @param h Pointer to an lt_handle_t to use (for encryption and enqueuing).
 * @param chunk_count Count of the L3 Command chunks. Only single chunk supported now.
 * @param corrupt_crc If true, CRC will be intentionally corrupted (e.g., to test CRC handling).
 * @param custom_resp_frame Optional custom 5-byte L2 response frame. If NULL, a default
 * frame with STATUS=REQ_OK and zero response length is used.
 *
 * @return LT_OK on success, or an appropriate lt_ret_t error code on failure.
 */
lt_ret_t mock_l3_command_responses(lt_handle_t *h, const size_t chunk_count, const bool corrupt_crc,
                                   const uint8_t custom_resp_frame[5]);

#ifdef __cplusplus
}
#endif

#endif  // LT_MOCK_HELPERS_H