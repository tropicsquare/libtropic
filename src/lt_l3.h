#ifndef LT_L3_H
#define LT_L3_H

/**
 * @defgroup group_l3_functions Layer 3 functions
 * @brief Used internally
 * @details Function used during l3 operation.
 *
 * @{
 */

/**
 * @file lt_l3.h
 * @brief Layer 3 functions declarations
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

/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define L3_PAIRING_KEY_EMPTY                0x15u
/** @brief L3 RESULT ﬁeld Value returned from pairing_key_read */
#define L3_PAIRING_KEY_INVALID              0x16u


/**
 * @details  Initializes nonce in handle to 0. This function is used during secure handshake.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_nonce_init(lt_handle_t *h);

/**
 * @details Increases by one nonce stored in handle. This function is called after successfull 
 *          reception of L3 response.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l3_nonce_increase(lt_handle_t *h);

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
