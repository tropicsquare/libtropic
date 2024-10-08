#ifndef LT_L2_FRAME_CHECK_H
#define LT_L2_FRAME_CHECK_H

/**
 * @defgroup group_l2_frame_check_functions Layer 2 frame check functions
 * @brief Used internally
 * @details Functions for L2 frame checking
 *
 * @{
 */

/**
 * @file lt_l2_frame_check.h
 * @brief Layer 2 frame check functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

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
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_frame_check(const uint8_t *frame);

/** @} */ // end of group_l2_frame_check_functions

#endif
