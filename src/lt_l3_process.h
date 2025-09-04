#ifndef LT_L3_TRANSFER_H
#define LT_L3_TRANSFER_H

/**
 * @defgroup group_l3_functions Layer 3 transfer functions
 * @brief Used internally
 * @details Function used during l3 operation.
 *
 * @{
 */

/**
 * @file lt_l3_process.h
 * @brief Layer 3 transfer functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/** @brief L3 RESULT ﬁeld Value */
#define TR01_L3_RESULT_OK 0xC3u
/** @brief L3 RESULT ﬁeld Value */
#define TR01_L3_RESULT_FAIL 0x3Cu
/** @brief L3 RESULT ﬁeld Value */
#define TR01_L3_RESULT_UNAUTHORIZED 0x01u
/** @brief L3 RESULT ﬁeld Value */
#define TR01_L3_RESULT_INVALID_CMD 0x02u

/** @brief L3 RESULT ﬁeld Value returned from ecc_key_read */
#define TR01_L3_ECC_INVALID_KEY 0x12u
/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define TR01_L3_PAIRING_KEY_EMPTY 0x15u
/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define TR01_L3_PAIRING_KEY_INVALID 0x16u
/** @brief The slot is already written in */
#define TR01_L3_R_MEM_DATA_WRITE_WRITE_FAIL 0x10
/** @brief The writing operation limit is reached for the slot. */
#define TR01_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED 0x11
/** @brief Failure to update the specified Monotonic Counter. The Monotonic Counter is already at 0. */
#define TR01_L3_MCOUNTER_UPDATE_ERROR 0x13
/** @brief The Monotonic Counter detects an attack and is locked. The counter must be reinitialized. */
#define TR01_L3_MCOUNTER_COUNTER_INVALID 0x14

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

#ifdef TEST
/**
 * @brief Used to increase nonce
 *
 * @param nonce       4B long number used as nonce inside of Noise protocol
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
LT_STATIC lt_ret_t lt_l3_nonce_increase(uint8_t *nonce) __attribute__((warn_unused_result));
#endif

/**
 * @brief Invalidates host's session data
 *
 * @param s3          Structure holding l3 state
 */
void lt_l3_invalidate_host_session_data(lt_l3_state_t *s3);

/** @} */  // end of group_l3_functions group

#endif
