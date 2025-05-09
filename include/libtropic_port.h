#ifndef LT_LIBTROPIC_PORT_H
#define LT_LIBTROPIC_PORT_H

/**
 * @defgroup group_port_functions Layer 1 port specific functions
 * @brief Defined for each supported platform
 * @details Function used by host platform during l1 operations. Check 'hal/port/' folder to see what is supported. 
 *
 * @{
 */

/**
 * @file libtropic_port.h
 * @brief Header file with layer 1 interfaces which are defined based on host platform
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/**
 * @brief Platform defined init function. Init resources and set pins as needed.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise LT_FAIL.
 */
lt_ret_t lt_port_init(lt_handle_t *h);

/**
 * @brief Platform defined deinit function. Deinit resources and clear pins as needed.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_deinit(lt_handle_t *h);

/**
 * @brief Set chip select pin low, platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_csn_low(lt_handle_t *h);

/**
 * @brief Set chip select pin high, platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_csn_high(lt_handle_t *h);

/**
 * @brief Do l1 transfer, platform defined function.
 *
 * @param h           Chip's handle
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout     Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_spi_transfer(lt_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform defined function for delay, specifies what host platform should do when libtropic's functions need some delay
 *
 * @param h           Chip's handle
 * @param ms          Time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_delay(lt_handle_t *h, uint32_t ms);

/**
 * @brief Fill buffer with random bytes, platform defined function.
 *
 * @param buff        Buffer to be filled
 * @param len         number of 32bit numbers
 * @return lt_ret_t   S_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len);

/** @} */ // end of group_port_functions

#endif
