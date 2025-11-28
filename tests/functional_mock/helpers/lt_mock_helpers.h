#ifndef LT_MOCK_HELPERS_H
#define LT_MOCK_HELPERS_H

/**
 * @file lt_mock_helpers.h
 * @brief Helper functions for functional mock tests.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds CRC16 to the end of the mocked response buffer.
 *
 * @param resp_buf Pointer to the response buffer where CRC will be appended. The buffer must have enough space for the
 * CRC (2 bytes).
 */
void add_resp_crc(void *resp_buf);

/**
 * @brief Calculates the total length of a mocked response buffer, including CHIP_STATUS, STATUS, RSP_LEN, RSP_DATA, and
 * CRC.
 *
 * @param resp_buf Pointer to the response buffer.
 * @return size_t Total length of the mocked response buffer.
 */
size_t calc_mocked_resp_len(const void *resp_buf);

/**
 * @brief Mock all data required to initialize Libtropic with lt_init().
 *
 * @details Currently, lt_init() initializes attributes based on the Application FW version running on TROPIC01.
 * The version is obtained from the chip. This function mocks the necessary responses to simulate a specific FW version.
 * As this operation needs to be done in each test that does any communication, this helper function is provided to
 * simplify the process.
 *
 * @param h Pointer to the lt_handle_t structure.
 * @param riscv_fw_ver Array representing the desired RISC-V FW version to mock.
 *
 * @return lt_ret_t LT_OK on success, error code otherwise.
 */
lt_ret_t mock_init_communication(lt_handle_t *h, const uint8_t riscv_fw_ver[4]);

#ifdef __cplusplus
}
#endif

#endif  // LT_MOCK_HELPERS_H