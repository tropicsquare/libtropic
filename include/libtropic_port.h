#ifndef LT_LIBTROPIC_PORT_H
#define LT_LIBTROPIC_PORT_H

/**
 * @defgroup group_port_functions 7. HAL Interface
 * @brief Functions defined for each supported platform.
 * @details Function used by host platform during hardware-specific operations. Check 'hal/' folder to see what is
 * supported. All of these functions have to be impemented by the port for libtropic to work.
 *
 * @{
 */

/**
 * @file libtropic_port.h
 * @brief Header file with HAL interfaces which are defined based on host platform
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdlib.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Max length of a path to a device in the device tree.
 */
#define LT_DEVICE_PATH_MAX_LEN 256

/**
 * @brief Platform defined init function. Init resources and set pins as needed.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_init(lt_l2_state_t *s2);

/**
 * @brief Platform defined deinit function. Deinit resources and clear pins as needed.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_deinit(lt_l2_state_t *s2);

/**
 * @brief Sets chip select pin low, platform defined function.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2);

/**
 * @brief Sets chip select pin high, platform defined function.
 *
 * @param s2          Structure holding l2 state
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2);

/**
 * @brief Does L1 transfer, platform defined function.
 *
 * @param s2          Structure holding l2 state
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout_ms  Timeout
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms);

/**
 * @brief Platform defined function for delay, specifies what host platform should do when libtropic's functions need
 * some delay.
 *
 * @param s2          Structure holding l2 state
 * @param ms          Time to wait in miliseconds
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms);

#if LT_USE_INT_PIN
/**
 * @brief Platform defined function used to specify reading of an interrupt pin, used as a signal that chip has a
 * response.
 *
 * @param s2          Structure holding l2 state
 * @param ms          Max time to wait in miliseconds
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms);
#endif
/**
 * @brief Fill buffer with random bytes, platform defined function.
 * @note This function should use some cryptographically secure mechanism to generate the random bytes. Its speed should
 * not be a concern, as this function is not called often.
 *
 * @param s2          Structure holding l2 state
 * @param buff        Buffer to be filled
 * @param count       Number of random bytes
 *
 * @retval            LT_OK   Function executed successfully
 * @retval            LT_FAIL Function did not execute successully
 */
lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count);

/**
 * @brief Port-specific logging function.
 * @note  The caller should not append any newline characters. This function is exlusively used by the logging macros in
 * libtropic_logging.h, that always append "\r\n" at the end.
 *
 * @param format      Pointer to a null-terminated byte string specifying how to interpret the data
 * @param ...         Arguments specifying data to print
 */
void lt_port_log(const char *format, ...);

/** @} */  // end of group_port_functions

#ifdef __cplusplus
}
#endif

#endif  // LT_LIBTROPIC_PORT_H
