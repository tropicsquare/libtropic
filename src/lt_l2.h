#ifndef LT_L2_H
#define LT_L2_H

/**
 * @defgroup group_l2_functions Layer 2 functions
 * @brief Used internally
 * @details Function used during l2 operation.
 *
 * @{
 */

/**
 * @file lt_l2.h
 * @brief Layer 2 functions declarations
 * @author Tropic Square s.r.o.
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

/**
 * @brief This function executes generic l2 request/respond. Prior calling this function, place 
 *        request's data into handle's iternal l2 buffer, structures defined in lt_l2_api_structs.h migh help.
 *        Handle's internal buffer will contain response's when function returns TS_OK.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_transfer(lt_handle_t *h);

/**
 * @brief This function executes generic L3 command. It expects command's data to be correctly
 *        encrypted using keys created during previsously called lt_l2_handshake_req().
 *        
 *        Prior calling this function, place 
 *        command's data into handle's iternal l3 buffer, structures defined in lt_l3_api_structs.h migh help.
 *        Handle's internal l3 buffer will contain result's data when function returns TS_OK.
 * 
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_encrypted_cmd(lt_handle_t *h);

/** @} */ // end of group_l2_functions

#endif
