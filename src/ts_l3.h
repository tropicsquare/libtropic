#ifndef TS_L3_H
#define TS_L3_H

/**
* @file ts_l3.h
* @brief This file contains interfaces related to layer 3.
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"

/**
 * @defgroup group_l3_functions [L3 functions]
 * @ingroup group_private_API
 * @brief Functions controlling l3 transmission
 * @details Function used during l3 operation.
 *
 * @{
 */

/** L3 RESULT ﬁeld Value */
#define L3_RESULT_OK                        0xC3u
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_FAIL                      0x3Cu
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_UNAUTHORIZED              0x01u
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_INVALID_CMD               0x02u


/**
 * @details  Initializes nonce in handle to 0. This function is used during secure handshake.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_nonce_init(ts_handle_t *h);

/**
 * @details Increases by one nonce stored in handle. This function is used after successfull reception of L3 response.
 * , uint16_t cmd_len,
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_nonce_increase(ts_handle_t *h);

/**
 * @details Perform l3 encrypted command operation.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_cmd(ts_handle_t *h);

/** @} */ // end of group_l3_functions
#endif
