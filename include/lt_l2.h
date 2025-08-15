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
 *        request's data into handle's iternal l2 buffer, structures defined in lt_l2_api_structs.h migh help with
 * encoding data.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l2_send(lt_l2_state_t *s2);

/**
 * @brief This function sends Resend_Req to request the last frame again.
 *        It is used in lt_l2_receive when a problem is detected with the last frame received.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l2_resend_response(lt_l2_state_t *s2);

/**
 * @brief This function receives l2 response. After successfull execution, hanlde's l2_buff will contain response.
 *        Structures defined in lt_l2_api_structs.h migh help with decoding.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l2_receive(lt_l2_state_t *s2);

/**
 * @brief This function sends content of encrypted l3 command's buffer over l2 layer. Data are sent from handle's
 * l3_buff. Use only after secure session was established with lt_session_start()
 *
 * @param s2          Structure holding l2 state
 * @param buff        Buffer containing encrypted l3 command
 * @param max_len     Maximal length of buff. Whole buffer might be used, or just its part.
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l2_send_encrypted_cmd(lt_l2_state_t *s2, uint8_t *buff, uint16_t max_len);

/**
 * @brief This function receives encrypted l3 response over l2 layer. Data are received into handle's l3_buff.
 *  Use only after secure session was established with lt_session_start()
 *
 * @param s2          Structure holding l2 state
 * @param buff        Buffer where encrypted l3 result will be stored
 * @param max_len     Maximal length of buff. Whole buffer might be used, or just its part.
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully
 */
lt_ret_t lt_l2_recv_encrypted_res(lt_l2_state_t *s2, uint8_t *buff, uint16_t max_len);
/** @} */  // end of group_l2_functions

#endif
