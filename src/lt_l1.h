#ifndef LT_L1_H
#define LT_L1_H

/**
 * @defgroup group_l1_functions Layer 1 functions
 * @brief Used internally
 * @details Function used internally by libtropic during l1 operation.
 *
 * @{
 */

/**
 * @file lt_l1.h
 * @brief Layer 1 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/** This bit in CHIP_STATUS byte signalizes that chip is ready to accept requests */
#define CHIP_MODE_READY_bit 0x01
/** This bit in CHIP_STATUS byte signalizes that chip is in ALARM mode */
#define CHIP_MODE_ALARM_bit 0x02
/** This bit in CHIP_STATUS byte signalizes that chip is in STARTUP mode */
#define CHIP_MODE_STARTUP_bit 0x04

/** Max number of GET_INFO requests when chip is not answering */
#define LT_L1_READ_MAX_TRIES 50
/** Number of ms to wait between each GET_INFO request */
#define LT_L1_READ_RETRY_DELAY 25

/** Minimal timeout when waiting for activity on SPI bus */
#define LT_L1_TIMEOUT_MS_MIN 5
/** Default timeout when waiting for activity on SPI bus */
#define LT_L1_TIMEOUT_MS_DEFAULT 70
/** Maximal timeout when waiting for activity on SPI bus */
#define LT_L1_TIMEOUT_MS_MAX 150

/** Get response request's ID */
#define GET_RESPONSE_REQ_ID 0xAA

/**
 * @brief Reads data from TROPIC01 into host platform
 *
 * @param s2          Structure holding l2 state
 * @param max_len     Max len of receive buffer
 * @param timeout_ms  Timeout - how long function will wait for response
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_read(lt_l2_state_t *s2, const uint32_t max_len, const uint32_t timeout_ms)
    __attribute__((warn_unused_result));

/**
 * @brief Writes data from host platform into TROPIC01
 *
 * @param s2          Structure holding l2 state
 * @param len         Length of data to send
 * @param timeout_ms  Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_l1_write(lt_l2_state_t *s2, const uint16_t len, const uint32_t timeout_ms)
    __attribute__((warn_unused_result));

/** @} */  // end of group_l1_functions

#endif
