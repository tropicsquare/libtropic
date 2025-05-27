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
 * @brief This function sends l2 request. Prior calling this function, place
 *        request's data into handle's iternal l2 buffer, structures defined in lt_l2_api_structs.h migh help with encoding data.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_send(lt_handle_t *h);

/**
 * @brief This function receives l2 response. After successfull execution, hanlde's l2_buff will contain response.
 *        Structures defined in lt_l2_api_structs.h migh help with decoding.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_receive(lt_handle_t *h);

/**
 * @brief This function sends content of encrypted l3 command's buffer over l2 layer. Data are sent from handle's l3_buff.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_send_encrypted_cmd(lt_handle_t *h);


/**
 * @brief This function receives encrypted l3 response over l2 layer. Data are received into handle's l3_buff.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l2_recv_encrypted_res(lt_handle_t *h);
/** @} */ // end of group_l2_functions

#endif
