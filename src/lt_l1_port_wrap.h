#ifndef LT_L1_PORT_WRAP_H
#define LT_L1_PORT_WRAP_H

/**
 * @defgroup group_l1_functions Layer 1 functions
 * @brief Used internally
 * @details Function used internally by libtropic during l1 operation.
 *
 * @{
 */

/**
 * @file lt_l1_port_wrap.h
 * @brief Layer 1 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/**
 * @brief Initialize handle and l1. This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_init(lt_l2_state_t *s2);

/**
 * @brief Wipe handle and deinitialize l1. This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_deinit(lt_l2_state_t *s2);

/**
 * @brief Sets chip select pin low. This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_csn_low(lt_l2_state_t *s2);

/**
 * @brief Set chip select pin high. This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_csn_high(lt_l2_state_t *s2);

/**
 * @brief Do l1 transfer. This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param tx_len      The length of data to be transferred
 * @param timeout     Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform's definition for delay, specifies what host
 *        platform should do when libtropic's functions need some delay.
 *        This is wrapper for platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @param ms          Time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_delay(lt_l2_state_t *s2, uint32_t ms);

#if LT_USE_INT_PIN
/**
 * @brief Specify what platform should do when waiting for signal from INT pin
 *
 * @param s2          Structure holding l2 state
 * @param ms          Maximal time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_delay_on_int(lt_l2_state_t *s2, uint32_t ms);
#endif

/** @} */  // end of group_l1_functions

#endif
