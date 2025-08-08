/**
 * @file libtropic_port.h
 * @brief Header file with layer 1 interfaces which are defined based on host platform
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifndef LT_LIBTROPIC_PORT_H
#define LT_LIBTROPIC_PORT_H

#include "libtropic_common.h"

/**
 * @brief Max length of a path to a device in the device tree.
 */
#define DEVICE_PATH_MAX_LEN 256

/**
 * @defgroup group_port_functions Layer 1 port specific functions
 * @brief Defined for each supported platform.
 * @details Function used by host platform during l1 operations. Check 'hal/port/' folder to see what is supported.
 *          All of these functions have to be impemented by the port for libtropic to work.
 *
 * @{
 */

/**
 * @brief Platform defined init function. Init resources and set pins as needed.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise LT_FAIL.
 */
lt_ret_t lt_port_init(lt_l2_state_t *s2);

/**
 * @brief Platform defined deinit function. Deinit resources and clear pins as needed.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_deinit(lt_l2_state_t *s2);

/**
 * @brief Set chip select pin low, platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2);

/**
 * @brief Set chip select pin high, platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2);

/**
 * @brief Do l1 transfer, platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout     Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform defined function for delay, specifies what host platform should do when libtropic's functions need
 * some delay
 *
 * @param s2          Structure holding l2 state
 * @param ms          Time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms);

#if LT_USE_INT_PIN
/**
 * @brief Platform definde function used to specify reading of int pin, used as a signal that chip has a response
 *
 * @param s2          Structure holding l2 state
 * @param ms          Max time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms);
#endif
/**
 * @brief Fill buffer with random bytes, platform defined function.
 *
 * @param buff        Buffer to be filled
 * @param len         number of 32bit numbers
 * @return lt_ret_t   LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len);

/** @} */  // end of group_port_functions

#endif
