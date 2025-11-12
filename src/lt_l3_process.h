#ifndef LT_L3_TRANSFER_H
#define LT_L3_TRANSFER_H

/**
 * @defgroup group_l3_functions 4.1. Layer 3: Encrypt/Decrypt
 * @brief Functions for Layer 3 packet encrypting/decrypting
 *
 * @{
 */

/**
 * @file lt_l3_process.h
 * @brief Layer 3 transfer functions declarations
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Common L3 Command result codes
 * @brief L3 Command result codes used by all commands.
 * @{
 */
/** @brief L3 Command executed successfully. */
#define TR01_L3_RESULT_OK 0xC3
/** @brief Generic L3 command failure. */
#define TR01_L3_RESULT_FAIL 0x3C
/** @brief Unauthorized access. */
#define TR01_L3_RESULT_UNAUTHORIZED 0x01
/** @brief Invalid or unsupported L3 command identifier. */
#define TR01_L3_RESULT_INVALID_CMD 0x02
/** @} */

/**
 * @name Specific L3 Command result codes
 * @brief L3 Command result codes specific to only some of the commands.
 * @{
 */
/** @brief The target slot is not empty when expected to be. */
#define TR01_L3_RESULT_SLOT_NOT_EMPTY 0x10
/** @brief The target FLASH slot has expired. */
#define TR01_L3_RESULT_SLOT_EXPIRED 0x11
/** @brief The key in selected slot is invalid or corrupted. */
#define TR01_L3_RESULT_INVALID_KEY 0x12
/** @brief Update operation failed (i.e. mcounter done). */
#define TR01_L3_RESULT_UPDATE_ERR 0x13
/** @brief The counter is disabled or has failed. */
#define TR01_L3_RESULT_COUNTER_INVALID 0x14
/** @brief The requested slot is empty and contains no valid data. */
#define TR01_L3_RESULT_SLOT_EMPTY 0x15
/** @brief The slot content is invalidated. */
#define TR01_L3_RESULT_SLOT_INVALID 0x16
/** @brief A hardware error occurred during a write operation. */
#define TR01_L3_RESULT_HARDWARE_FAIL 0x17
/** @} */

/**
 * @brief Encrypts content of L3 buffer and fills it with cyphertext ready to be sent to TROPIC01.
 * @note This function expects that L3 buffer is already filled with data to be sent.
 *
 * @param s3          Structure holding l3 state
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l3_encrypt_request(lt_l3_state_t *s3) __attribute__((warn_unused_result));

/**
 * @brief Decrypts response from TROPIC01 and fills L3 buffer with decrypted data.
 * @note This function is used after encrypted l3 payload was received from TROPIC01.
 *
 * @param s3          Structure holding l3 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_decrypt_response(lt_l3_state_t *s3) __attribute__((warn_unused_result));

/**
 * @brief Invalidates host's session data
 *
 * @param s3          Structure holding l3 state
 */
void lt_l3_invalidate_host_session_data(lt_l3_state_t *s3);

/** @} */  // end of group_l3_functions group

#ifdef __cplusplus
}
#endif

#endif  // LT_L3_TRANSFER_H
