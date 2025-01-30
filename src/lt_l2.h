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
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

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
 *        Shall be executed only on device which has active secure channel.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_encrypted_cmd(lt_handle_t *h);

/** @} */ // end of group_l2_functions

#endif
