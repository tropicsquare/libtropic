#ifndef TS_L1_H
#define TS_L1_H

/**
* @file ts_l1.h
* @brief Layer 1 interfaces
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"

/** This bit in CHIP_STATUS byte signalizes that chip is ready to accept requests */
#define CHIP_MODE_READY_bit 0x01
/** This bit in CHIP_STATUS byte signalizes that chip is in ALARM mode */
#define CHIP_MODE_ALARM_bit 0x02
/** This bit in CHIP_STATUS byte signalizes that chip is in STARTUP mode */
#define CHIP_MODE_STARTUP_bit 0x04

/** Max number of GET_INFO requests when chip is not answering */
#define TS_L1_READ_MAX_TRIES 10
/** Number of ms to wait between each GET_INFO request */
#define TS_L1_READ_RETRY_DELAY 25

/** Minimal timeout when waiting for activity on SPI bus */
#define TS_L1_TIMEOUT_MS_MIN 5
/** Default timeout when waiting for activity on SPI bus */
#define TS_L1_TIMEOUT_MS_DEFAULT 70
/** Maximal timeout when waiting for activity on SPI bus */
#define TS_L1_TIMEOUT_MS_MAX 150

/** Maximal delay in ms which can be used in delay funtion */
#define TS_L1_DELAY_MS_MAX 500

/** Get info request's ID */
#define GET_INFO_REQ_ID 0xAA

/**
 * @brief Initialize handle and l1
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l1_init(ts_handle_t *h);

/**
 * @brief Wipe handle and deinitialize l1
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l1_deinit(ts_handle_t *h);

/**
 * @brief Read data from Tropic chip into host platform
 *
 * @param h           Chip's handle
 * @param max_len     Max len of receive buffer
 * @param timeout     Timeout - how long function will wait for response
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l1_read(ts_handle_t *h, const uint32_t max_len, const uint32_t timeout);

/**
 * @brief Write data from host platform into Tropic chip
 *
 * @param h           Chip's handle
 * @param len         Length of data to send
 * @param timeout     Timeout
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l1_write(ts_handle_t *h, const uint16_t len, const uint32_t timeout);

#endif
