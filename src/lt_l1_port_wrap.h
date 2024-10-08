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
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_init(lt_handle_t *h);

/**
 * @brief Wipe handle and deinitialize l1. This is wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_deinit(lt_handle_t *h);

/**
 * @brief Sets chip select pin low. This is wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_csn_low(lt_handle_t *h);

/**
 * @brief Set chip select pin high. This is wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_csn_high(lt_handle_t *h);

/**
 * @brief Do l1 transfer. This is wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout     Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_spi_transfer(lt_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform's definition for delay, specifies what host
 *        platform should do when libtropic's functions need some delay.
 *        This is wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @param ms          Time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_delay(lt_handle_t *h, uint32_t ms);

/** @} */ // end of group_l1_functions

#endif
