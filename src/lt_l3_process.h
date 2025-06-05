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
 * @file lt_l3_transfer.h
 * @brief Layer 3 transfer functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/** @brief L3 RESULT ﬁeld Value */
#define L3_RESULT_OK                        0xC3u
/** @brief L3 RESULT ﬁeld Value */
#define L3_RESULT_FAIL                      0x3Cu
/** @brief L3 RESULT ﬁeld Value */
#define L3_RESULT_UNAUTHORIZED              0x01u
/** @brief L3 RESULT ﬁeld Value */
#define L3_RESULT_INVALID_CMD               0x02u

/** @brief L3 RESULT ﬁeld Value returned from ecc_key_read */
#define L3_ECC_INVALID_KEY                  0x12u
/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define L3_PAIRING_KEY_EMPTY                0x15u
/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define L3_PAIRING_KEY_INVALID              0x16u

/**
 * @brief 
 * 
 * @param s3          Structure holding l3 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_encrypt_request(lt_l3_state_t *s3);

/**
 * @brief 
 * 
 * @param s3          Structure holding l3 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_decrypt_response(lt_l3_state_t *s3);

//-----------------

/**
 * @details Perform l3 encrypted command operation. It takes handle's l3 buffer containing l3 command
 *          and after successfull execution handle's l3 buffer contains l3 result.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_cmd(lt_handle_t *h);

/** @} */ // end of group_l3_functions

#endif
