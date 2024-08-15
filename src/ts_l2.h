#ifndef TS_L2_H
#define TS_L2_H

/**
* @file ts_l2.h
* @brief Layer 2 interfaces
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"


/**
 * @defgroup group_l2_functions Layer 2 functions
 * @brief Used internally
 * @details Function used during l2 operation.
 *
 * @{
 */

/** @brief STATUS ﬁeld value */
#define L2_STATUS_REQUEST_OK       0x01
/** @brief STATUS ﬁeld value */
#define L2_STATUS_RESULT_OK        0x02
/** @brief STATUS ﬁeld value */
#define L2_STATUS_REQUEST_CONT     0x03
/** @brief STATUS ﬁeld value */
#define L2_STATUS_RESULT_CONT      0x04
/** @brief STATUS ﬁeld value */
#define L2_STATUS_HSK_ERR          0x79
/** @brief STATUS ﬁeld value */
#define L2_STATUS_NO_SESSION       0x7A
/** @brief STATUS ﬁeld value */
#define L2_STATUS_TAG_ERR          0x7B
/** @brief STATUS ﬁeld value */
#define L2_STATUS_CRC_ERR          0x7C
/** @brief STATUS ﬁeld value */
#define L2_STATUS_UNKNOWN_ERR      0x7E
/** @brief STATUS ﬁeld value */
#define L2_STATUS_GEN_ERR          0x7F
/** @brief STATUS ﬁeld value */
#define L2_STATUS_NO_RESP          0xFF

/**
 * @brief This function checks if incomming L2 frame is valid
 *
 * @param             frame
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_frame_check(const uint8_t *frame);

/**
 * @brief
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_transfer(ts_handle_t *h);

/**
 * @brief This function executes generic L3 command. It expects command's data correctly encrypted using keys created during previsously called ts_l2_handshake_req()
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_encrypted_cmd(ts_handle_t *h);

/** @} */ // end of group_l2_functions

#endif
